#pragma once

#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "hal/ledc_types.h"
#include "i2c_lcd_pcf8574.h"
#include "math.h"
#include "memory.h"
#include "menu_manager.h"
#include "nvs.h"
#include "sdkconfig.h"
#include "stdint.h"
#include "string.h"

#define PERCENT_TO_13_BIT_CUSTOM(percent)                                      \
  ((uint32_t)(50 * pow((8191 / 50), (percent) / 100.0)))

extern i2c_lcd_pcf8574_handle_t lcd;

/**
 * @brief Semaphore to control the display
 */
extern SemaphoreHandle_t sDisplay;

/**
 * @brief Variable to store the current brightness
 */
extern char brightnessLabel[16];

/**
 * @brief Menu function that control the brightness of backlight
 *
 * @param args Arguments unused
 */
void fMenu_brightnessControl(void *args);

/**
 * @brief A function to initialize the I2C, LCD and PWM of backlight
 *
 * @return ESP_OK if successful
 */
esp_err_t display_init(void);
