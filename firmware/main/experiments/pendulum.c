#include "pendulum.h"

const static char *TAG = "pendulum";

static char *print_option_pendulum(uint8_t selected) {
  char option_str[22];
  static char hist_str[6];
  int count = 0;
  pcnt_unit_get_count(pcnt_unit, &count);

  count = (count - 1) / 2;

  snprintf(option_str, sizeof(option_str), "Periods: n\03%02d/n\03%02d",
           count % 100, selected % 100);

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 1, 1);
  lcd_print(&lcd, option_str);
  lcd_set_cursor(&lcd, 16, 1);
  xSemaphoreGive(sDisplay);

  snprintf(hist_str, sizeof(hist_str), "Pen%02u", selected % 100);
  return hist_str;
}

static counter_config_t mk_counter_config_pendulum(uint8_t value) {

  value = value * 2 + 1;

  counter_config_t counter_config = {
      .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
      .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
      .filter = {.max_glitch_ns = 100},
      .watchPoint = {1, value},
      .resetCounter = NULL,
  };

  ESP_LOGI(TAG, "Pendulum period: %d", value);

  return counter_config;
}

void fMenu_pendulum(void *args) {
  experiment_config_t pendulum = {
      .title = "Pendulum",
      .default_value = CONFIG_PENDULUM,
      .interval_options = {1, 99},
      .check_obstruct = true,
      .print_option = &print_option_pendulum,
      .mk_counter_config = &mk_counter_config_pendulum,
  };

  run_experiment(pendulum);
}
