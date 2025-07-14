#include "menus.h"
#include "centripetal.h"
#include "classic.h"
#include "display.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "history.h"
#include "i2c_lcd_pcf8574.h"
#include "mechanical.h"
#include "menu_manager.h"
#include "misc.h"
#include "pendulum.h"
#include "sdkconfig.h"
#include "spring.h"
#include "stdint.h"
#include "string.h"

static const char *TAG = "menu";

static uint8_t first;
static uint8_t end;

/**
 * @brief Render menu on the display
 *
 * @param current_path Currente state of menu
 */
static void showDisplay(menu_path_t *current_path) {
  uint8_t select = current_path->current_index;
  uint8_t num_options = current_path->current_menu->num_options;
  char *title = current_path->current_menu->label;
  uint8_t count = 1;

  if (select < first || select == 0) {
    first = select;
    end = first + CONFIG_DISPLAY_ROWS - 1;
  } else if (select + 1 > end) {
    end = select + 1;
    first = end - CONFIG_DISPLAY_ROWS + 1;
  }

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_clear(&lcd);
  lcd_set_cursor(&lcd, (CONFIG_DISPLAY_COLUMNS - (strlen(title))) / 2, 0);
  lcd_print(&lcd, title);

  for (uint8_t _ = first; _ < end && _ < num_options; _++) {
    lcd_set_cursor(&lcd, 0, count);
    count++;
    if (_ == select) {
      lcd_print(&lcd, "\x7E");
    }
    lcd_print(&lcd, current_path->current_menu->submenus[_].label);
  }
  if (first > 0) {
    lcd_set_cursor(&lcd, CONFIG_DISPLAY_COLUMNS - 1, 1);
    lcd_print(&lcd, "\xA5");
  }
  if (end < num_options) {
    lcd_set_cursor(&lcd, CONFIG_DISPLAY_COLUMNS - 1, CONFIG_DISPLAY_ROWS - 1);
    lcd_print(&lcd, "\xA5");
  }
  xSemaphoreGive(sDisplay);
}

// Submenu settings
menu_node_t mSettings[2] = {
    {
        .label = brightnessLabel,
        .num_options = 0,
        .function = &fMenu_brightnessControl,
    },
    {
        .label = (char *)"Info",
        .num_options = 0,
        .function = &fMenu_Info,
    },
};

// Submenu to functions that work like an old photogate
menu_node_t mClassic[3] = {
    {
        .label = (char *)"Pend",
        .num_options = 0,
        .function = &fMenu_classic_pend,
    },
    {
        .label = (char *)"Pulse",
        .num_options = 0,
        .function = &fMenu_classic_pulse,
    },
    {
        .label = (char *)"Gate",
        .num_options = 0,
        .function = &fMenu_classic_gate,
    },
};

// Root menu
menu_node_t mRootOption[7] = {
    {
        .label = (char *)"Classic",
        .submenus = mClassic,
        .num_options = 3,
    },
    {
        .label = (char *)"Pendulum",
        .num_options = 0,
        .function = &fMenu_pendulum,
    },
    {
        .label = (char *)"Spring",
        .num_options = 0,
        .function = &fMenu_spring,
    },
    {
        .label = (char *)"Mechanical Energy",
        .num_options = 0,
        .function = &fMenu_mechanical,
    },
    {
        .label = (char *)"Centripetal",
        .num_options = 0,
        .function = &fMenu_centripetal,
    },
    {
        .label = (char *)"History",
        .num_options = 0,
        .function = &fMenu_history,
    },
    {
        .label = (char *)"Settings",
        .submenus = mSettings,
        .num_options = 2,
    },
};

// Create the root menu
static const menu_node_t mRoot = {
    .label = (char *)"Photogate V2",
    .submenus = mRootOption,
    .num_options = 7,
};

static menu_config_t menuConfig = {
    .root = mRoot,
    .display = &showDisplay,
    .loop = false,
};

esp_err_t menuInit(void) {

  ESP_LOGI(TAG, "Initializing menu");

  xTaskCreatePinnedToCore(menu_init, "menu_manager", 4096, &menuConfig, 1, NULL,
                          0);
  return ESP_OK;
}
