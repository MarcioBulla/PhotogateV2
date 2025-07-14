#include "mechanical.h"

const static char *TAG = "mechanical";

static const char *shape_options[4] = {
    "Solid",
    "R\02+R\01",
    "2R\02  ",
    "2R\01  ",
};
static const counter_config_t shape_configs[4] = {
    {
        .watchPoint = {1, 2},
        .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
    },
    {
        .watchPoint = {1, 2},
        .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
    },
    {
        .watchPoint = {2, 3},
        .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
    },
    {
        .watchPoint = {1, 4},
        .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
    },
};

static char *print_option_mechanical(uint8_t selected) {
  static char hist_str[6];
  char option_str[13];

  snprintf(option_str, 13, "Shape: %s", shape_options[selected]);

  ESP_LOGI(TAG, "%s", option_str);

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 1, 1);
  lcd_print(&lcd, option_str);
  lcd_set_cursor(&lcd, 7, 1);
  xSemaphoreGive(sDisplay);

  snprintf(hist_str, 5, "%s", shape_options[selected]);

  return hist_str;
}

static counter_config_t mk_counter_config_mechanical(uint8_t value) {
  counter_config_t counter_config = {
      .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
      .filter = {.max_glitch_ns = 50},
      .resetCounter = NULL,
  };

  const counter_config_t *config_temp = &shape_configs[value];
  counter_config.watchPoint[0] = config_temp->watchPoint[0];
  counter_config.watchPoint[1] = config_temp->watchPoint[1];
  counter_config.falling = config_temp->falling;

  ESP_LOGI(TAG, "Mechanical config created");

  return counter_config;
}

void fMenu_mechanical(void *args) {
  experiment_config_t mechanical = {
      .title = "Mechanical Energy",
      .default_value = CONFIG_ENERGY,
      .interval_options = {0, 3},
      .check_obstruct = true,
      .print_option = &print_option_mechanical,
      .mk_counter_config = &mk_counter_config_mechanical,
  };

  run_experiment(mechanical);
}
