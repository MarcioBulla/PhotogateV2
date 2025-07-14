#pragma once

#include "counter.h"
#include "display.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "freertos/semphr.h"
#include "hal/pcnt_types.h"
#include "history.h"
#include "i2c_lcd_pcf8574.h"
#include "menu_manager.h"
#include "misc.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include "stdint.h"
#include "time.h"

typedef enum {
  EXPERIMENT_CONFIG = 0,
  EXPERIMENT_WAITTING,
  EXPERIMENT_TIMING,
  EXPERIMENT_DONE,
  EXPERIMENT_EXIT,
  EXPERIMENT_ERROR,
} experiment_stage_t;

typedef struct {
  char *title;
  uint8_t default_value;
  char *(*print_option)(uint8_t selected);
  counter_config_t (*mk_counter_config)(uint8_t varlue);
  uint8_t interval_options[2];
  bool check_obstruct;
} experiment_config_t;

void run_experiment(experiment_config_t exp_config);
