#include "misc.h"

const static char *TAG = "misc";

void fMenu_stub(void *args) {

  Navigate_t command = NAVIGATE_NOTHING;

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_clear(&lcd);
  lcd_set_cursor(&lcd, 0, 0);
  lcd_print(&lcd, "Working");
  lcd_set_cursor(&lcd, 0, 1);
  lcd_print(&lcd, "Please wait");
  xSemaphoreGive(sDisplay);

  while (true) {
    xQueueReceive(qCommands, &command, portMAX_DELAY);

    xSemaphoreTake(sDisplay, portMAX_DELAY);
    if (command == NAVIGATE_UP) {
      lcd_scroll_display_right(&lcd);
      xSemaphoreGive(sDisplay);
    } else if (command == NAVIGATE_DOWN) {
      lcd_scroll_display_left(&lcd);
      xSemaphoreGive(sDisplay);
    } else if (command == NAVIGATE_SELECT) {
      xSemaphoreGive(sDisplay);
      ESP_LOGI(TAG, "Clicked");
    } else if (command == NAVIGATE_BACK) {
      xSemaphoreGive(sDisplay);
      exitFunction();
    }

    ESP_LOGI(TAG, "RECEIVED COMMAND %d", command);
  }
}

void fMenu_Info(void *args) {
  Navigate_t command = NAVIGATE_NOTHING;

  char *info_text[4] = {
      "Version: 2.1",
      "Developers: Marcio Bulla",
      "Repository: github.com/MarcioBulla/PhotogateV2",
      "Site: PhotogateV2.MarcioBulla.com",
  };
  size_t max_scroll[4] = {0};
  size_t min_position[4] = {0};

  for (uint8_t i = 0; i < 4; i++) {
    size_t len = strlen(info_text[i]);
    max_scroll[i] = (len > 20) ? len + 1 - 20 : 0;
  }

  char text_line[21];

  while (true) {
    xSemaphoreTake(sDisplay, portMAX_DELAY);
    lcd_clear(&lcd);

    for (uint8_t _ = 0; _ < 4; _++) {
      size_t position = (max_scroll[_] > 0) ? min_position[_] : 0;
      strncpy(text_line, info_text[_] + position, 20);
      text_line[20] = '\0';

      lcd_set_cursor(&lcd, 0, _);
      lcd_print(&lcd, text_line);
    }
    xSemaphoreGive(sDisplay);

    xQueueReceive(qCommands, &command, portMAX_DELAY);

    switch (command) {
    case NAVIGATE_BACK:
      exitFunction();
      break;

    case NAVIGATE_UP:
      for (uint8_t _ = 0; _ < 4; _++) {
        if (min_position[_] < max_scroll[_])
          min_position[_]++;
      }
      break;

    case NAVIGATE_DOWN:
      for (uint8_t _ = 0; _ < 4; _++) {
        if (min_position[_] > 0) {
          min_position[_]--;
        }
      }
      break;

    case NAVIGATE_SELECT:
      memset(min_position, 0, sizeof(min_position));
      break;

    default:
      ESP_LOGW(TAG, "Comando desconhecido: %d", command);
      break;
    }
  }
}

const static uint8_t HOURGLASS_COLL = 3;
const static uint8_t HOURGLASS_ROW = 2;
static TaskHandle_t tHourGlass = NULL;

static void HourGlass_animation(void *args) {
  while (true) {
    for (uint8_t ch = 4; ch < 8; ch++) {
      if (xSemaphoreTake(sDisplay, portMAX_DELAY) == pdTRUE) {
        lcd_set_cursor(&lcd, HOURGLASS_COLL, HOURGLASS_ROW);
        lcd_write(&lcd, ch);
        xSemaphoreGive(sDisplay);
      }
      vTaskDelay(pdMS_TO_TICKS(750));
    }
  }
}

void startHourGlassAnimation(void) {
  ESP_LOGI(TAG, "Starting HourGlass animation");
  xTaskCreate(HourGlass_animation, "HourGlass", 1024, NULL, 1, &tHourGlass);
}

void stopHourGlassAnimation(uint8_t frame) {

  if (tHourGlass != NULL) {
    ESP_LOGI(TAG, "HourGlass animation already stopped");
    vTaskDelete(tHourGlass);
    tHourGlass = NULL;
  }

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, HOURGLASS_COLL, HOURGLASS_ROW);
  lcd_write(&lcd, frame % 4 + 4);
  xSemaphoreGive(sDisplay);
  ESP_LOGI(TAG, "Hourglass stooped in frame %u", frame % 4);
}
