#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

extern nvs_handle_t nvsStorage;

/**
 * @brief Initializes the NVS (Non-Volatile Storage)
 *
 * @return ESP_OK if successful
 */
esp_err_t nvs_init(void);
