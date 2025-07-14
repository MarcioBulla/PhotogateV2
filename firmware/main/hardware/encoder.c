#include "encoder.h"
#include "iot_button.h"

static const char *TAG = "encoder";

/**
 * @brief Variable that sends to menu manager
 */
static Navigate_t command = NAVIGATE_NOTHING;

static button_handle_t btn = NULL;
static knob_handle_t knob = NULL;

static int8_t encoderDirection = 0;

static const button_config_t btn_config = {
    .long_press_time = 500,
    .short_press_time = 100,
};

static const button_gpio_config_t btn_gpio_config = {
    .gpio_num = CONFIG_ENCODER_SW,
    .active_level = 0,
    .disable_pull = true,
};

static const knob_config_t knob_config = {
    .default_direction = 0,
    .gpio_encoder_a = CONFIG_ENCODER_DT,
    .gpio_encoder_b = CONFIG_ENCODER_CLK,
};

static void button_click_cb(void *arg, void *usr_data) {

  ESP_LOGI(TAG, "BUTTON_CLICK");
  if (uxQueueSpacesAvailable(qCommands) > 0) {
    command = NAVIGATE_SELECT;
    // Send command to menu manager's queue
    xQueueSend(qCommands, &command, portMAX_DELAY);
  } else {
    // Case the queue is full
    ESP_LOGW(TAG, "Queue is full, cannot send NAVIGATE_SELECT command");
  }
}

static void button_long_press_cb(void *arg, void *usr_data) {
  ESP_LOGI(TAG, "BUTTON_LONG_PRESS");
  if (uxQueueSpacesAvailable(qCommands) > 0) {
    command = NAVIGATE_BACK;
    // Send command to menu manager's queue
    xQueueSend(qCommands, &command, portMAX_DELAY);
  } else {
    // Case the queue is full
    ESP_LOGW(TAG, "Queue is full, cannot send NAVIGATE_BACK command");
  }
}

static void knob_cw_cb(void *arg, void *usr_data) {

// Case your encoder sends double signal when you rotate it
#ifdef CONFIG_ENCODER_DOUBLE_SIGNAL
  if (encoderDirection < 0) {
    encoderDirection = 0;
  }
  encoderDirection++;
  if (encoderDirection == 2) {
    encoderDirection = 0;
#endif
    ESP_LOGI(TAG, "CLOCKWISE");
    if (uxQueueSpacesAvailable(qCommands) > 0) {
      command = NAVIGATE_UP;
      xQueueSend(qCommands, &command, portMAX_DELAY);
    } else {
      ESP_LOGW(TAG, "Queue is full, cannot send NAVIGATE_UP command");
    }
#ifdef CONFIG_ENCODER_DOUBLE_SIGNAL
  }
#endif
}

static void knob_ccw_cb(void *arg, void *usr_data) {

// Case your encoder sends double signal when you rotate it
#ifdef CONFIG_ENCODER_DOUBLE_SIGNAL
  if (encoderDirection > 0) {
    encoderDirection = 0;
  }
  encoderDirection--;
  if (encoderDirection == -2) {
    encoderDirection = 0;
#endif
    ESP_LOGI(TAG, "COUNTER_CLOCKWISE");
    if (uxQueueSpacesAvailable(qCommands) > 0) {
      command = NAVIGATE_DOWN;
      xQueueSend(qCommands, &command, portMAX_DELAY);
    } else {
      ESP_LOGW(TAG, "Queue is full, cannot send NAVIGATE_DOWN command");
    }
#ifdef CONFIG_ENCODER_DOUBLE_SIGNAL
  }
#endif
}

esp_err_t encoder_init(void) {
  esp_err_t ret = ESP_OK;
  ESP_LOGI(TAG, "Starting encoder");

  ret = iot_button_new_gpio_device(&btn_config, &btn_gpio_config, &btn);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to init button");
    return ESP_FAIL;
  }

  knob = iot_knob_create(&knob_config);

  if (knob == NULL) {
    ESP_LOGE(TAG, "Failed to init knob");
    return ESP_FAIL;
  }

  iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, button_click_cb, NULL);
  iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, NULL,
                         button_long_press_cb, NULL);

  iot_knob_register_cb(knob, KNOB_LEFT, knob_cw_cb, NULL);
  iot_knob_register_cb(knob, KNOB_RIGHT, knob_ccw_cb, NULL);

  return ESP_OK;
}
