#pragma once
#include "counter.h"
#include "cronos.h"
#include "display.h"
#include "driver/pulse_cnt.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/projdefs.h"
#include "hal/pcnt_types.h"
#include "i2c_lcd_pcf8574.h"
#include "pendulum.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>

void fMenu_classic_pend(void *args);

void fMenu_classic_pulse(void *args);

void fMenu_classic_gate(void *args);
