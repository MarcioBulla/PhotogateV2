#include "counter.h"

static const char *TAG = "counter";

/**
 * @brief Variable to config limits and priority.
 */
const static pcnt_unit_config_t unit_cfg = {
    .high_limit = 200,
    .low_limit = -1,
    .intr_priority = 1,
};

/**
 * @brief Variable to config channel
 */
const static pcnt_chan_config_t chan_cfg = {
    .edge_gpio_num = CONFIG_SENSOR_IR,
    .level_gpio_num = -1,
};

pcnt_unit_handle_t pcnt_unit;
pcnt_channel_handle_t pcnt_chan;
QueueHandle_t qPCNT;

/**
 * @brief watch point current configureted on counter
 */
static int32_t currentWatchers[2] = {0, 0};

/**
 * @brief Function on RAM memory that is executed in interrupts of watchpoints
 * if is needed reset counter.
 *
 * @param edata State of counter
 * @param user_ctx Custom user argument
 * @param time_now Time in micro seconds of interrupts
 * @return If successfully
 */
static BaseType_t (*resetCounter)(const pcnt_watch_event_data_t *edata,
                                  void *user_ctx, int64_t time_now) = NULL;

/**
 * @brief Fun
 *
 * @param unit unit of counter
 * @param edata State of counter
 * @param user_ctx Custom user argument
 * @return If is successfully the send queue value
 */
static bool IRAM_ATTR cronos(pcnt_unit_handle_t unit,
                             const pcnt_watch_event_data_t *edata,
                             void *user_ctx) {
  int64_t ts = esp_timer_get_time();

  BaseType_t high_task_wakeup = pdFALSE;

  if (resetCounter) {
    high_task_wakeup = resetCounter(edata, user_ctx, ts);
  } else {
    xQueueSendFromISR((QueueHandle_t)user_ctx, &ts, &high_task_wakeup);
  }

  return (high_task_wakeup == pdTRUE);
}

esp_err_t counter_init(void) {
  ESP_LOGI(TAG, "Initializing counter module");

  qPCNT = xQueueCreate(5, sizeof(int64_t));
  if (qPCNT == NULL) {
    ESP_LOGE(TAG, "Failed to create PCNT queue");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "PCNT queue created successfully");

  esp_err_t ret = pcnt_new_unit(&unit_cfg, &pcnt_unit);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create PCNT unit: %s", esp_err_to_name(ret));
    return ret;
  }
  ESP_LOGI(TAG, "PCNT unit created");

  ret = pcnt_new_channel(pcnt_unit, &chan_cfg, &pcnt_chan);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create PCNT channel: %s", esp_err_to_name(ret));
    return ret;
  }
  ESP_LOGI(TAG, "PCNT channel created on GPIO %d", CONFIG_SENSOR_IR);

  ret =
      pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                   PCNT_CHANNEL_EDGE_ACTION_HOLD);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set edge action: %s", esp_err_to_name(ret));
    return ret;
  }
  ESP_LOGI(TAG, "Edge action configured (increase on rising edge)");

  pcnt_event_callbacks_t cbs = {
      .on_reach = cronos,
  };

  ret = pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, qPCNT);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to register callbacks: %s", esp_err_to_name(ret));
    return ret;
  }
  ESP_LOGI(TAG, "Event callbacks registered");

  ret = pcnt_unit_add_watch_point(pcnt_unit, unit_cfg.high_limit);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to add watch point: %s", esp_err_to_name(ret));
    return ret;
  }
  ESP_LOGI(TAG, "Watch point added at limit %d", unit_cfg.high_limit);

  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
  ESP_LOGI(TAG, "PCNT unit enabled");

  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
  ESP_LOGI(TAG, "PCNT counter cleared");

  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
  ESP_LOGI(TAG, "PCNT unit started - counter module ready");

  return ESP_OK;
}

void pcnt_config_counter(counter_config_t config_counter) {

  ESP_LOGI(TAG, "Configuring PCNT experiment");

  ESP_ERROR_CHECK(pcnt_unit_disable(pcnt_unit));
  ESP_LOGI(TAG, "PCNT unit disabled for reconfiguration");

  esp_err_t ret = pcnt_channel_set_edge_action(pcnt_chan, config_counter.rising,
                                               config_counter.falling);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set edge action: %s", esp_err_to_name(ret));
    return;
  }
  ESP_LOGI(TAG, "Edge actions configured (rising: %d, falling: %d)",
           config_counter.rising, config_counter.falling);

  ret = pcnt_unit_set_glitch_filter(pcnt_unit, &config_counter.filter);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set glitch filter: %s", esp_err_to_name(ret));
    return;
  }
  ESP_LOGI(TAG, "Glitch filter configured");

  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
  ESP_LOGI(TAG, "PCNT unit enabled");

  for (uint8_t i = 0; i < 2; i++) {
    if (currentWatchers[i] != 0) {
      ret = pcnt_unit_remove_watch_point(pcnt_unit, currentWatchers[i]);
      if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Removed watch point: %" PRId32, currentWatchers[i]);
      } else {
        ESP_LOGW(TAG, "Failed to remove watch point %" PRId32 ": %s",
                 currentWatchers[i], esp_err_to_name(ret));
      }
    }

    if (config_counter.watchPoint[i] != 0) {
      ret = pcnt_unit_add_watch_point(pcnt_unit, config_counter.watchPoint[i]);
      if (ret == ESP_OK) {
        currentWatchers[i] = config_counter.watchPoint[i];
        ESP_LOGI(TAG, "Set watch point: %" PRId32,
                 config_counter.watchPoint[i]);
      } else {
        ESP_LOGE(TAG, "Failed to add watch point %" PRId32 ": %s",
                 config_counter.watchPoint[i], esp_err_to_name(ret));
        currentWatchers[i] = 0;
      }
    } else {
      currentWatchers[i] = 0;
    }
  }
  resetCounter = config_counter.resetCounter;

  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
  ESP_LOGI(TAG, "PCNT counter cleared");

  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
  ESP_LOGI(TAG, "PCNT experiment configuration complete");
}
