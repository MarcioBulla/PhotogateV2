#include "classic.h"

const static char *TAG = "classic";

static volatile bool send = true;

static const char *classic_options[2] = {
    "Read",
    "Off ",
};

static char *classsic_names[3] = {"Pend ", "Pulse", "Gate "};

static char *print_option_classic(uint8_t selected) {
  char option_str[21];

  snprintf(option_str, sizeof(option_str), "Memory: %s",
           classic_options[selected % 2]);

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 1, 1);
  lcd_print(&lcd, option_str);
  lcd_set_cursor(&lcd, 8, 1);
  xSemaphoreGive(sDisplay);

  return classsic_names[selected / 2];
}

BaseType_t IRAM_ATTR reset_pend(const pcnt_watch_event_data_t *edata,
                                void *user_ctx, int64_t time_now) {
  BaseType_t high_task_wakeup = pdFALSE;

  if (edata->watch_point_value == 1) {
    if (send) {
      xQueueSendFromISR((QueueHandle_t)user_ctx, &time_now, &high_task_wakeup);
    } else {
      pcnt_unit_clear_count(pcnt_unit);
      high_task_wakeup = pdTRUE;
    }
    send = !send;
  } else if (edata->watch_point_value == 3) {
    xQueueSendFromISR((QueueHandle_t)user_ctx, &time_now, &high_task_wakeup);
    pcnt_unit_clear_count(pcnt_unit);
  }

  return (high_task_wakeup == pdTRUE);
}

BaseType_t IRAM_ATTR reset_classic(const pcnt_watch_event_data_t *edata,
                                   void *user_ctx, int64_t time_now) {
  BaseType_t high_task_wakeup = pdFALSE;

  if (edata->watch_point_value == 2) {
    pcnt_unit_clear_count(pcnt_unit);
  }

  xQueueSendFromISR((QueueHandle_t)user_ctx, &time_now, &high_task_wakeup);

  return (high_task_wakeup == pdTRUE);
}

static const counter_config_t classic_configs[6] = {
    {
        .watchPoint = {1, 3},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
        .filter = {.max_glitch_ns = 100},
        .resetCounter = NULL,
    },
    {
        .watchPoint = {1, 3},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
        .filter = {.max_glitch_ns = 100},
        .resetCounter = &reset_pend,
    },
    {
        .watchPoint = {1, 2},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
        .filter = {.max_glitch_ns = 100},
        .resetCounter = NULL,
    },
    {
        .watchPoint = {1, 2},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
        .filter = {.max_glitch_ns = 100},
        .resetCounter = &reset_classic,
    },
    {
        .watchPoint = {1, 2},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .filter = {.max_glitch_ns = 50},
        .resetCounter = NULL,
    },
    {
        .watchPoint = {1, 2},
        .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        .filter = {.max_glitch_ns = 50},
        .resetCounter = &reset_classic,
    },
};

static counter_config_t mk_counter_config_classic(uint8_t value) {

  ESP_LOGI(TAG, "Classic config created");
  return classic_configs[value];
}

void fMenu_classic_pend(void *args) {
  experiment_config_t classic = {
      .title = "Pend",
      .default_value = CONFIG_CLASSIC_PEND,
      .interval_options = {0, 1},
      .check_obstruct = true,
      .print_option = &print_option_classic,
      .mk_counter_config = &mk_counter_config_classic,
  };

  run_experiment(classic);
}

void fMenu_classic_pulse(void *args) {
  experiment_config_t classic = {
      .title = "Pulse",
      .default_value = CONFIG_CLASSIC_PULSE + 2,
      .interval_options = {2, 3},
      .check_obstruct = true,
      .print_option = &print_option_classic,
      .mk_counter_config = &mk_counter_config_classic,
  };

  run_experiment(classic);
}

void fMenu_classic_gate(void *args) {
  experiment_config_t classic = {
      .title = "Gate",
      .default_value = CONFIG_CLASSIC_GATE + 4,
      .interval_options = {4, 5},
      .check_obstruct = true,
      .print_option = &print_option_classic,
      .mk_counter_config = &mk_counter_config_classic,
  };

  run_experiment(classic);
}
