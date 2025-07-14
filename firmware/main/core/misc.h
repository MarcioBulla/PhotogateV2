#pragma once

#include "display.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "i2c_lcd_pcf8574.h"
#include "menu_manager.h"
#include "portmacro.h"
#include "stdint.h"
#include "string.h"

/**
 * @brief Generic function to be called by menu
 *
 * @param args Argument unused
 */
void fMenu_stub(void *args);

/**
 * @brief Function to show info of Photogate
 *
 * @param args Argument unused
 */
void fMenu_Info(void *args);

void startHourGlassAnimation(void);

void stopHourGlassAnimation(uint8_t frame);
