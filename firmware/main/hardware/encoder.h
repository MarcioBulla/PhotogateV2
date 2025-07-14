#pragma once

#include "button_gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "iot_button.h"
#include "iot_knob.h"
#include "menu_manager.h"
#include "sdkconfig.h"
#include "soc/gpio_num.h"

/**
 * @brief A function to initialize the encoder
 *
 * @return ESP_OK if successful
 */
esp_err_t encoder_init(void);
