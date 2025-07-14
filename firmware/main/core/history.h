#pragma once
#include "display.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "i2c_lcd_pcf8574.h"
#include "menu_manager.h"
#include "portmacro.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct {
  char *time;
  char *type;
} hist_data_t;

void add_reading(hist_data_t reading);

void fMenu_history(void *args);
