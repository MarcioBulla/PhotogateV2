#include "history.h"

static const char *TAG = "history";

#define CAPACITY 100
static size_t size = 0;
static hist_data_t history[CAPACITY];

static void print_history_line(uint8_t index, uint8_t row) {
  char data[21];
  snprintf(data, 21, "%02u|%s |%s", index, history[index].time,
           history[index].type);

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, row);
  lcd_print(&lcd, data);
  xSemaphoreGive(sDisplay);
}

static void remove_reading(size_t index) {
  free(history[index].time);
  free(history[index].type);
  size--;
  for (uint8_t _ = index; _ < size; _++) {
    history[_] = history[_ + 1];
  }
  ESP_LOGI(TAG, "History removed");
}

void add_reading(hist_data_t reading) {
  if (size < CAPACITY) {
    history[size].time = strdup(reading.time);
    history[size].type = strdup(reading.type);
    size++;
    ESP_LOGI(TAG, "Added reading %d", size);
  }
}

void fMenu_history(void *args) {
  Navigate_t commands = NAVIGATE_NOTHING;

  uint8_t first = 0;
  uint8_t end = 2;
  uint8_t row = 1;

  uint8_t selected = 0;
  uint8_t row_selected = 0;

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_clear(&lcd);
  lcd_print(&lcd, "n\03|Time(s)    |Type ");
  lcd_blink(&lcd);
  xSemaphoreGive(sDisplay);

  while (size > 0) {

    ESP_LOGI(TAG, "First: %u", first);
    ESP_LOGI(TAG, "Selected: %u", selected);
    ESP_LOGI(TAG, "End: %u", end);

    for (uint8_t _ = first; _ <= end; _++) {
      if (_ < size) {
        print_history_line(_, row);
      } else {
        xSemaphoreTake(sDisplay, portMAX_DELAY);
        lcd_set_cursor(&lcd, 0, row);
        lcd_print(&lcd, "                    ");
        xSemaphoreGive(sDisplay);
      }
      if (_ == selected) {
        row_selected = row;
      }

      row++;
    }
    row = 1;

    xSemaphoreTake(sDisplay, portMAX_DELAY);
    lcd_set_cursor(&lcd, 0, row_selected);
    xSemaphoreGive(sDisplay);

    xQueueReceive(qCommands, &commands, portMAX_DELAY);

    switch (commands) {

    case NAVIGATE_UP:
      if (selected < size - 1) {
        selected++;
      }
      break;

    case NAVIGATE_DOWN:
      if (selected > 0) {
        selected--;
      }
      break;

    case NAVIGATE_SELECT:
      commands = NAVIGATE_NOTHING;

      xSemaphoreTake(sDisplay, portMAX_DELAY);
      lcd_set_cursor(&lcd, 0, row_selected);
      lcd_print(&lcd, "Two Clicks to Remove");
      lcd_set_cursor(&lcd, 0, row_selected);
      xSemaphoreGive(sDisplay);

      xQueueReceive(qCommands, &commands, pdMS_TO_TICKS(2000));
      if (commands == NAVIGATE_SELECT) {
        ESP_LOGI(TAG, "deleting %d", selected);
        selected = selected == size ? size - 1 : selected;
        remove_reading(selected);
      }
      break;

    case NAVIGATE_BACK:
      xSemaphoreTake(sDisplay, portMAX_DELAY);
      lcd_no_blink(&lcd);
      xSemaphoreGive(sDisplay);
      exitFunction();
      break;

    default:
      ESP_LOGW(TAG, "Unknown command");
    }
    if (selected < first) {
      first--;
      end--;
    }
    if (selected > end) {
      first++;
      end++;
    }
  }

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_no_blink(&lcd);
  lcd_clear(&lcd);
  xSemaphoreGive(sDisplay);

  while (true) {

    xSemaphoreTake(sDisplay, portMAX_DELAY);
    lcd_set_cursor(&lcd, 9, 1);
    lcd_print(&lcd, "No");
    lcd_set_cursor(&lcd, 6, 2);
    lcd_print(&lcd, "Readings");
    lcd_set_cursor(&lcd, 3, 3);
    lcd_print(&lcd, "Click to Exit");
    xSemaphoreGive(sDisplay);

    xQueueReceive(qCommands, &commands, portMAX_DELAY);
    if (commands == NAVIGATE_SELECT || commands == NAVIGATE_BACK) {
      exitFunction();
    }
  }
}
