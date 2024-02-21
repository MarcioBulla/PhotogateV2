#include "freertos/projdefs.h"
#include "hal/pcnt_types.h"
#include <driver/pulse_cnt.h>
#include <encoder.h>
#include <esp_log.h>
#include <esp_random.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <hd44780.h>
#include <i2cdev.h>
#include <menu_manager.h>
#include <pcf8574.h>
#include <sdkconfig.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <main.h>

#define H_POSITION_HOURGLASS 3
#define V_POSITION_HOURGLASS 2

static const char *TAG = "main";

menu_node_t root = {
    .label = "Main  Menu",
    .num_options = 5,
    .submenus = root_options,
};

menu_node_t root_options[5] = {
    {.label = "Pendulum", .function = &Pendulum},
    {.label = "Spring", .function = &Spring},
    {.label = "Energy", .function = &Energy},
    {.label = "History", .function = &History},
    {.label = "Settings", .submenus = settings_options, .num_options = 3},
};

menu_node_t settings_options[3] = {
    {.label = "Menu Type: 1", .function = &Change_menu},
    {.label = "Brightness: 100%", .function = &Brightness},
    {.label = "Info", .function = &Info},
};

menu_config_t config_menu;

void app_main(void) {

  config_menu.root = root;
  config_menu.input = &map;
  config_menu.display = &displayNormal;
  config_menu.loop = false;

  ESP_ERROR_CHECK(startLCD());
  ESP_ERROR_CHECK(startEncoder());
  ESP_ERROR_CHECK(startPCNT());

  xTaskCreatePinnedToCore(&menu_init, "menu_init", 2048, &config_menu, 1, NULL,
                          0);
  vTaskDelete(NULL);
}

int32_t currentWatchers[2] = {-1, -1};

TaskHandle_t hHourglass = NULL;
SemaphoreHandle_t sDisplay = NULL;
pcnt_unit_handle_t pcnt_unit = NULL;
pcnt_channel_handle_t pcnt_chan = NULL;
QueueHandle_t qPCNT = NULL;
QueueHandle_t qEncoder;
QueueHandle_t qCommand;

static rotary_encoder_t re = {
    .pin_a = CONFIG_ENCODER_CLK,
    .pin_b = CONFIG_ENCODER_DT,
    .pin_btn = CONFIG_ENCODER_SW,
};

esp_err_t startEncoder(void) {
  qEncoder = xQueueCreate(5, sizeof(rotary_encoder_event_t));
  qCommand = xQueueCreate(5, sizeof(rotary_encoder_event_t));

  ESP_ERROR_CHECK(rotary_encoder_init(qEncoder));
  ESP_ERROR_CHECK(rotary_encoder_add(&re));
  return ESP_OK;
}

static i2c_dev_t pcf8574;

static esp_err_t write_lcd_data(const hd44780_t *lcd, uint8_t data) {
  return pcf8574_port_write(&pcf8574, data);
}

hd44780_t lcd = {.write_cb = write_lcd_data,
                 .font = HD44780_FONT_5X8,
                 .lines = CONFIG_VERTICAL_SIZE,
                 .pins = {
                     .rs = 0,
                     .e = 2,
                     .d4 = 4,
                     .d5 = 5,
                     .d6 = 6,
                     .d7 = 7,
                     .bl = 3,
                 }};

static const uint8_t char_data[] = {
    // Right Arrow - 0
    0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00,
    // char e - 1
    0x00, 0x00, 0x00, 0x0C, 0x12, 0x1C, 0x10, 0x0E,
    // char i - 2
    0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x0C,
    // Number Symbol - 3
    0x0C, 0x12, 0x12, 0x0C, 0x00, 0x1E, 0x00, 0x00,
    // HourGlass 1 - 4
    0x1F, 0x1F, 0x0E, 0x04, 0x04, 0x0A, 0x11, 0x1F,
    // HourGlass 2 - 5
    0x1F, 0x1B, 0x0E, 0x04, 0x04, 0x0E, 0x11, 0x1F,
    // HourGlass 3 - 6
    0x1F, 0x11, 0x0E, 0x04, 0x04, 0x0E, 0x1B, 0x1F,
    // HourGlass 4 - 7
    0x1F, 0x11, 0x0A, 0x04, 0x04, 0x0E, 0x1F, 0x1F};

esp_err_t startLCD(void) {
  ESP_ERROR_CHECK(i2cdev_init());
  vSemaphoreCreateBinary(sDisplay);
  ESP_ERROR_CHECK(pcf8574_init_desc(&pcf8574, CONFIG_DISPLAY_ADDR, 0,
                                    CONFIG_I2C_SDA, CONFIG_I2C_SCL));

  hd44780_switch_backlight(&lcd, true);
  ESP_ERROR_CHECK(hd44780_init(&lcd));
  for (uint8_t i = 0; i < 8; i++) {
    hd44780_upload_character(&lcd, i, char_data + (i * 8));
  }
  ESP_LOGI(TAG, "LCD ON!");

  return ESP_OK;
}

void HourGlass_animation(void *args) {
  while (true) {
    for (uint8_t i = 4; i < 8; i++) {
      xSemaphoreTake(sDisplay, portMAX_DELAY);
      hd44780_gotoxy(&lcd, H_POSITION_HOURGLASS, V_POSITION_HOURGLASS);
      hd44780_putc(&lcd, i);
      xSemaphoreGive(sDisplay);
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}

Navigate_t map(void) {
  rotary_encoder_event_t e;
  do {
    xQueueReceive(qEncoder, &e, portMAX_DELAY);

  } while (e.type == RE_ET_BTN_PRESSED || e.type == RE_ET_BTN_RELEASED);

  if (xSemaphoreTake(Menu_mutex, 0) == pdTRUE) {
    xSemaphoreGive(Menu_mutex);

    switch (e.type) {

    case RE_ET_BTN_CLICKED:
      ESP_LOGI(TAG, "SELECT");
      return NAVIGATE_SELECT;

    case RE_ET_BTN_LONG_PRESSED:
      ESP_LOGI(TAG, "BACK");
      return NAVIGATE_BACK;

    case RE_ET_CHANGED:
      if (e.diff > 0) {
        ESP_LOGI(TAG, "UP");
        return NAVIGATE_UP;
      } else {
        ESP_LOGI(TAG, "DOWN");
        return NAVIGATE_DOWN;
      }
    default:
      ESP_LOGI(TAG, "NOTHIN");
    }
  } else if (e.type == RE_ET_BTN_LONG_PRESSED) {
    if (hHourglass != NULL) {
      vTaskDelete(hHourglass);
      hHourglass = NULL;
    }
    for (uint8_t i = 0; i < 2; i++) {
      if (currentWatchers[i] > 0) {
        ESP_ERROR_CHECK(
            pcnt_unit_remove_watch_point(pcnt_unit, currentWatchers[i]));
      }
      currentWatchers[i] = -1;
    }

    ESP_LOGI(TAG, "BACK");
    return NAVIGATE_BACK;
  } else {
    xQueueSend(qCommand, &e, 0);
  }
  return NAVIGATE_NOTHIN;
}

uint8_t first = 0, end = 0;
const char *old_title;

void displayNormal(menu_path_t *current_path) {
  hd44780_control(&lcd, true, false, false);

  uint8_t select = current_path->current_index;
  uint8_t count = 1;
  char *title = current_path->current_menu->label;

  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, (CONFIG_HORIZONTAL_SIZE - strlen(title)) / 2, 0);
  hd44780_puts(&lcd, title);
  if (select < first || select == 0) {
    first = select;
    end = first + CONFIG_VERTICAL_SIZE - 1;
  } else if (select + 1 > end || old_title != title) {
    end = select + 1;
    first = end - CONFIG_VERTICAL_SIZE + 1;
  }
  old_title = title;

  for (uint8_t _ = first; _ < end; _++) {
    hd44780_gotoxy(&lcd, 0, count);
    count++;
    if (_ == select) {
      hd44780_putc(&lcd, 0b01111110);
      hd44780_putc(&lcd, ' ');
    }
    hd44780_puts(&lcd, current_path->current_menu->submenus[_].label);
  }
}

void displayLoop(menu_path_t *current_path) {
  hd44780_control(&lcd, true, false, false);

  char *title = current_path->current_menu->label;

  uint8_t select = current_path->current_index;
  uint8_t prev = (current_path->current_menu->num_options + select - 1) %
                 current_path->current_menu->num_options;
  uint8_t next = (select + 1) % current_path->current_menu->num_options;

  const char *prev_label = current_path->current_menu->submenus[prev].label;
  const char *select_label = current_path->current_menu->submenus[select].label;
  const char *next_label = current_path->current_menu->submenus[next].label;

  uint8_t central_title = (CONFIG_HORIZONTAL_SIZE - strlen(title)) / 2;
  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, central_title, 0);
  hd44780_puts(&lcd, title);
  hd44780_gotoxy(&lcd, 0, 1);
  hd44780_puts(&lcd, prev_label);
  hd44780_gotoxy(&lcd, 0, 2);
  hd44780_putc(&lcd, 0b01111110);
  hd44780_putc(&lcd, ' ');
  hd44780_puts(&lcd, select_label);
  hd44780_gotoxy(&lcd, 0, 3);
  hd44780_puts(&lcd, next_label);
}

// Experiments
pcnt_unit_config_t config_unit = {
    .high_limit = 200,
    .low_limit = -10,
};

pcnt_chan_config_t config_chan = {
    .edge_gpio_num = CONFIG_SENSOR_IR,
};

static bool cronos(pcnt_unit_handle_t pcnt_unit,
                   const pcnt_watch_event_data_t *edata, void *user_ctx) {
  BaseType_t high_task_wakeup;
  QueueHandle_t qPCNT = (QueueHandle_t)user_ctx;
  time_t temp_time = esp_timer_get_time();
  xQueueSendFromISR(qPCNT, &temp_time, &high_task_wakeup);
  return (high_task_wakeup == pdTRUE);
};

pcnt_event_callbacks_t pcnt_event = {
    .on_reach = cronos,
};

esp_err_t startPCNT(void) {
  qPCNT = xQueueCreate(2, sizeof(time_t));
  pcnt_new_unit(&config_unit, &pcnt_unit);
  pcnt_new_channel(pcnt_unit, &config_chan, &pcnt_chan);

  pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_HOLD,
                               PCNT_CHANNEL_EDGE_ACTION_HOLD);

  pcnt_unit_register_event_callbacks(pcnt_unit, &pcnt_event, qPCNT);

  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));

  return ESP_OK;
}

void pcnt_config_experiment(experiment_config_t config_experiment) {
  ESP_ERROR_CHECK(pcnt_unit_disable(pcnt_unit));

  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(
      pcnt_chan, config_experiment.rising, config_experiment.falling));

  ESP_ERROR_CHECK(
      pcnt_unit_set_glitch_filter(pcnt_unit, &config_experiment.filter));

  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));

  for (uint8_t i = 0; i < 2; i++) {
    if (currentWatchers[i] > 0) {
      ESP_ERROR_CHECK(
          pcnt_unit_remove_watch_point(pcnt_unit, currentWatchers[i]));
      ESP_LOGI(TAG, "Remove watch point: %" PRId32, currentWatchers[i]);
    }
    ESP_ERROR_CHECK(
        pcnt_unit_add_watch_point(pcnt_unit, config_experiment.watchPoint[i]));
    currentWatchers[i] = config_experiment.watchPoint[i];
    ESP_LOGI(TAG, "Set watch point: %" PRId32, config_experiment.watchPoint[i]);
  }

  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));

  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

void print_config(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  hd44780_gotoxy(&lcd, 0, 3);
  hd44780_puts(&lcd, "     !!Config!!     ");
  xSemaphoreGive(sDisplay);
}

void print_waitting(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  hd44780_gotoxy(&lcd, 0, 3);
  hd44780_puts(&lcd, "    !!Waitting!!    ");
  xSemaphoreGive(sDisplay);
}

void print_timing(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  hd44780_gotoxy(&lcd, 0, 3);
  hd44780_puts(&lcd, "     !!Timing!!     ");
  xSemaphoreGive(sDisplay);
}

void print_done(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  hd44780_gotoxy(&lcd, 0, 3);
  hd44780_puts(&lcd, "      !!Done!!      ");
  xSemaphoreGive(sDisplay);
}

void periods_to_string(uint8_t periods, char *string) {
  string[0] = '0' + periods / 10;
  string[1] = '0' + periods % 10;
  string[2] = '\0';
}

void micro_to_second(time_t microsecond, char *string) {
  double seconds = microsecond / 1000000.0;
  unsigned int int_seconds = (unsigned int)seconds;
  unsigned int milliseconds = (unsigned int)((seconds - int_seconds) * 1000);
  unsigned int microseconds_part =
      (unsigned int)(((seconds - int_seconds) * 1000 - milliseconds) * 1000);
  snprintf(string, 12, "%03d,%03d.%03d", int_seconds, milliseconds,
           microseconds_part);
}

void update_periods(char *current_periods_str) {
  xQueueSemaphoreTake(sDisplay, 0);
  hd44780_gotoxy(&lcd, 12, 1);
  hd44780_puts(&lcd, current_periods_str);
  xSemaphoreGive(sDisplay);
}

void update_time(time_t first, time_t lest) {
  char time_str[13];
  micro_to_second(lest - first, time_str);
  time_str[11] = 's';
  time_str[12] = '\0';
  xQueueSemaphoreTake(sDisplay, 0);
  hd44780_gotoxy(&lcd, 5, 2);
  hd44780_puts(&lcd, time_str);
  xSemaphoreGive(sDisplay);
}

void back_to_config(rotary_encoder_event_type_t event,
                    experiment_stage_t *stage, experiment_config_t config) {
  if (event == RE_ET_BTN_CLICKED) {
    *stage = EXPERIMENT_CONFIG;

    pcnt_unit_stop(pcnt_unit);
    update_time(0, 0);
    event = RE_ET_BTN_RELEASED;
    vTaskDelete(hHourglass);
    hHourglass = NULL;
    hd44780_gotoxy(&lcd, H_POSITION_HOURGLASS, V_POSITION_HOURGLASS);
    hd44780_putc(&lcd, 7);
  }
}

void Pendulum(void *args) {
  rotary_encoder_event_t e;
  experiment_data_t data;
  int count = 0;
  uint8_t set_periods = CONFIG_PENDULUM;
  char set_periods_str[3];
  char current_periods_str[3];
  time_t first = 0, lest = 0;
  time_t time;
  experiment_stage_t stage = EXPERIMENT_CONFIG;
  experiment_config_t config = {
      .rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
      .falling = PCNT_CHANNEL_EDGE_ACTION_HOLD,
      .filter = {.max_glitch_ns = 100},
      .watchPoint[0] = 1,
  };

  periods_to_string(set_periods, set_periods_str);

  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, 6, 0);
  hd44780_puts(&lcd, "Pendulum");
  hd44780_gotoxy(&lcd, 1, 1);
  hd44780_puts(&lcd, "Periods: n\x03");
  hd44780_puts(&lcd, "00/n\x03");
  hd44780_gotoxy(&lcd, H_POSITION_HOURGLASS, V_POSITION_HOURGLASS);
  hd44780_putc(&lcd, 7);
  update_time(first, lest);

  while (true) {
    update_periods("00");
    print_config();
    hd44780_gotoxy(&lcd, 17, 1);
    hd44780_control(&lcd, true, true, false);

    while (stage == EXPERIMENT_CONFIG) {
      xSemaphoreTake(sDisplay, portMAX_DELAY);
      periods_to_string(set_periods, set_periods_str);
      hd44780_puts(&lcd, set_periods_str);
      hd44780_gotoxy(&lcd, 17, 1);
      xSemaphoreGive(sDisplay);

      xQueueReceive(qCommand, &e, portMAX_DELAY);

      if (e.type == RE_ET_CHANGED) {
        if (e.diff > 0) {
          if (set_periods < 99)
            set_periods++;
        } else {
          if (set_periods > 1)
            set_periods--;
        }
      } else if (e.type == RE_ET_BTN_CLICKED) {
        e.type = RE_ET_BTN_RELEASED;
        stage = EXPERIMENT_WAITTING;

        snprintf(data.option, 8, "Pen%02d", set_periods);

        hd44780_control(&lcd, true, false, false);
        config.watchPoint[1] = 2 * set_periods + 1;

        pcnt_config_experiment(config);

        print_waitting();

        xTaskCreatePinnedToCore(&HourGlass_animation, "HourGlass Animation",
                                2048, NULL, 1, &hHourglass, 0);
      }
    }
    while (stage == EXPERIMENT_WAITTING) {
      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        first = time;
        stage = EXPERIMENT_TIMING;
        print_timing();
      }
    }
    while (stage == EXPERIMENT_TIMING) {
      pcnt_unit_get_count(pcnt_unit, &count);
      periods_to_string((count - 1) / 2, current_periods_str);

      lest = esp_timer_get_time() + esp_random() % 10000;

      update_time(first, lest);
      update_periods(current_periods_str);

      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        stage = EXPERIMENT_DONE;

        print_done();

        lest = time;
        update_periods(set_periods_str);
        update_time(first, lest);
      }
    }
    while (stage == EXPERIMENT_DONE) {
      micro_to_second(lest - first, data.timed);
      append_history(data);

      xQueueReceive(qCommand, &e, portMAX_DELAY);
      back_to_config(e.type, &stage, config);
    }
  }
}

void Spring(void *args) {
  rotary_encoder_event_t e;
  experiment_data_t data;
  int count = 0;
  uint8_t set_periods = CONFIG_SPRING;
  char set_periods_str[3];
  char current_periods_str[3];
  time_t first = 0, lest = 0;
  time_t time;
  experiment_stage_t stage = EXPERIMENT_CONFIG;
  experiment_config_t config = {
      .rising = PCNT_CHANNEL_EDGE_ACTION_HOLD,
      .falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
      .filter = {.max_glitch_ns = 100},
      .watchPoint[0] = 1,
  };

  periods_to_string(set_periods, set_periods_str);

  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, 7, 0);
  hd44780_puts(&lcd, "Spring");
  hd44780_gotoxy(&lcd, 1, 1);
  hd44780_puts(&lcd, "Periods: n\x03");
  hd44780_puts(&lcd, "00/n\x03");
  hd44780_gotoxy(&lcd, H_POSITION_HOURGLASS, V_POSITION_HOURGLASS);
  hd44780_putc(&lcd, 7);
  update_time(first, lest);

  while (true) {
    update_periods("00");
    print_config();
    hd44780_control(&lcd, true, false, true);

    while (stage == EXPERIMENT_CONFIG) {
      xSemaphoreTake(sDisplay, portMAX_DELAY);
      periods_to_string(set_periods, set_periods_str);
      hd44780_gotoxy(&lcd, 17, 1);
      hd44780_puts(&lcd, set_periods_str);
      hd44780_gotoxy(&lcd, 16, 1);
      xSemaphoreGive(sDisplay);

      xQueueReceive(qCommand, &e, portMAX_DELAY);

      if (e.type == RE_ET_CHANGED) {
        if (e.diff > 0) {
          if (set_periods < 99)
            set_periods++;
        } else {
          if (set_periods > 1)
            set_periods--;
        }
      } else if (e.type == RE_ET_BTN_CLICKED) {
        e.type = RE_ET_BTN_RELEASED;
        stage = EXPERIMENT_WAITTING;

        hd44780_control(&lcd, true, false, false);
        config.watchPoint[1] = set_periods + 1;

        snprintf(data.option, 8, "Spr%02d", set_periods);

        pcnt_config_experiment(config);

        print_waitting();

        xTaskCreatePinnedToCore(&HourGlass_animation, "HourGlass Animation",
                                2048, NULL, 1, &hHourglass, 0);
      }
    }
    while (stage == EXPERIMENT_WAITTING) {
      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        first = time;
        stage = EXPERIMENT_TIMING;
        print_timing();
      }
    }
    while (stage == EXPERIMENT_TIMING) {
      pcnt_unit_get_count(pcnt_unit, &count);
      periods_to_string((count - 1), current_periods_str);

      lest = esp_timer_get_time() + esp_random() % 10000;

      update_time(first, lest);
      update_periods(current_periods_str);

      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        stage = EXPERIMENT_DONE;

        print_done();

        lest = time;
        update_periods(set_periods_str);
        update_time(first, lest);
      }
    }
    while (stage == EXPERIMENT_DONE) {
      micro_to_second(lest - first, data.timed);
      append_history(data);

      xQueueReceive(qCommand, &e, portMAX_DELAY);
      back_to_config(e.type, &stage, config);
    }
  }
}

void select_shape_energy(energy_t select, experiment_config_t *exp_config) {
  switch (select) {

  case ENERGY_SOLID:
    exp_config->rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->watchPoint[0] = 1;
    exp_config->watchPoint[1] = 2;
    break;

  case ENERGY_RIRE:
    exp_config->rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->falling = PCNT_CHANNEL_EDGE_ACTION_HOLD;
    exp_config->watchPoint[0] = 1;
    exp_config->watchPoint[1] = 2;
    break;

  case ENERGY_RI:
    exp_config->rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->watchPoint[0] = 2;
    exp_config->watchPoint[1] = 3;
    break;

  case ENERGY_RE:
    exp_config->rising = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->falling = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
    exp_config->watchPoint[0] = 1;
    exp_config->watchPoint[1] = 4;
    break;
  }
}

void print_shape_energy(energy_t selecte, char string[6]) {

  ESP_LOGI(TAG, "SHAPE: %d", selecte);
  switch (selecte) {

  case ENERGY_SOLID:
    strncpy(string, "Solid", 6);
    break;

  case ENERGY_RIRE:
    strncpy(string, "R\x02+R\x01", 6);
    break;

  case ENERGY_RI:
    strncpy(string, "2R\x02  ", 6);
    break;

  case ENERGY_RE:
    strncpy(string, "2R\x01  ", 6);
    break;
  }
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  hd44780_gotoxy(&lcd, 8, 1);
  hd44780_puts(&lcd, string);
  hd44780_gotoxy(&lcd, 7, 1);
  xSemaphoreGive(sDisplay);
}

void Energy(void *args) {
  rotary_encoder_event_t e;
  energy_t set_shape = (energy_t)CONFIG_ENERGY;
  time_t first = 0, lest = 0;
  time_t time;
  experiment_data_t data;
  experiment_stage_t stage = EXPERIMENT_CONFIG;
  experiment_config_t config = {
      .filter = {.max_glitch_ns = 100},
  };

  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, 1, 0);
  hd44780_puts(&lcd, "Mechanical  Energy");
  hd44780_gotoxy(&lcd, 1, 1);
  hd44780_puts(&lcd, "Shape: ");
  print_shape_energy(set_shape, data.option);
  hd44780_gotoxy(&lcd, H_POSITION_HOURGLASS, V_POSITION_HOURGLASS);
  hd44780_putc(&lcd, 7);
  update_time(first, lest);

  while (true) {
    print_config();
    hd44780_control(&lcd, true, false, true);

    while (stage == EXPERIMENT_CONFIG) {
      print_shape_energy(set_shape, data.option);

      xQueueReceive(qCommand, &e, portMAX_DELAY);

      if (e.type == RE_ET_CHANGED) {
        if (e.diff > 0) {
          if (set_shape < 3)
            set_shape++;
        } else {
          if (set_shape > 0)
            set_shape--;
        }
      } else if (e.type == RE_ET_BTN_CLICKED) {
        e.type = RE_ET_BTN_RELEASED;
        stage = EXPERIMENT_WAITTING;

        hd44780_control(&lcd, true, false, false);

        select_shape_energy(set_shape, &config);
        pcnt_config_experiment(config);

        print_waitting();

        xTaskCreatePinnedToCore(&HourGlass_animation, "HourGlass Animation",
                                2048, NULL, 1, &hHourglass, 0);
      }
    }
    while (stage == EXPERIMENT_WAITTING) {
      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        first = time;
        stage = EXPERIMENT_TIMING;
        print_timing();
      }
    }
    while (stage == EXPERIMENT_TIMING) {
      lest = esp_timer_get_time();

      update_time(first, lest);

      xQueueReceive(qCommand, &e, 0);
      back_to_config(e.type, &stage, config);
      if (xQueueReceive(qPCNT, &time, pdMS_TO_TICKS(40)) == pdTRUE) {
        stage = EXPERIMENT_DONE;

        print_done();

        lest = time;
        update_time(first, lest);
      }
    }
    while (stage == EXPERIMENT_DONE) {
      micro_to_second(lest - first, data.timed);
      append_history(data);

      xQueueReceive(qCommand, &e, portMAX_DELAY);
      back_to_config(e.type, &stage, config);
    }
  }
}

experiment_data_t data_history[99];
experiment_data_array_t history = {
    .size = 0,
    .capability = 100,
    .array = data_history,
};

void append_history(experiment_data_t data) {
  if (history.size < history.capability) {
    history.array[history.size] = data;
    history.size++;
  } else {
    for (size_t i = 0; i < history.capability; i++) {
      history.array[i] = history.array[i + 1];
    }
    history.array[history.size] = data;
  }
}
void remove_at_history(size_t index) {
  for (size_t i = index; i < history.size - 1; i++) {
    history.array[i] = history.array[i + 1];
  }

  history.size--;
}

void print_hist_data(size_t index, uint8_t line) {
  char string[23];
  snprintf(string, 23, "%02d|%s|%s", index, history.array[index].timed,
           history.array[index].option);

  hd44780_gotoxy(&lcd, 0, line);
  hd44780_puts(&lcd, string);
  hd44780_gotoxy(&lcd, 0, line);
}

void hd44780_clear_line(const hd44780_t *lcd, uint8_t line) {
  hd44780_gotoxy(lcd, 0, line);
  hd44780_puts(lcd, "                    ");
  hd44780_gotoxy(lcd, 0, line);
}

void History(void *args) {
  rotary_encoder_event_t e;
  uint8_t select_hist = 0;
  uint8_t count;
  uint8_t cursor_position = 0;
  uint8_t first_hist = 0, end_hist = 0;

  hd44780_clear(&lcd);
  hd44780_puts(&lcd, "n\x03");
  hd44780_puts(&lcd, "|Timed(s)   |Type");

  hd44780_control(&lcd, true, false, true);

  while (history.size > 0) {

    if (select_hist >= history.size) {
      select_hist = history.size - 1;
    }
    if (select_hist <= first_hist) {
      first_hist = select_hist;
      end_hist = first_hist + CONFIG_VERTICAL_SIZE - 2;
    } else if (select_hist > end_hist) {
      end_hist = select_hist;
      first_hist = end_hist - CONFIG_VERTICAL_SIZE + 2;
    }
    ESP_LOGI(TAG, "\n First: %02d\n Select: %02d\n End: %02d", first_hist,
             select_hist, end_hist);
    count = 1;

    for (uint8_t _ = first_hist; _ <= end_hist; _++) {
      if (_ < history.size) {
        print_hist_data(_, count);
        if (_ == select_hist) {
          cursor_position = count;
        }
      } else {
        hd44780_clear_line(&lcd, count);
      }

      count++;
    }

    hd44780_gotoxy(&lcd, 0, cursor_position);

    xQueueReceive(qCommand, &e, portMAX_DELAY);

    if (e.type == RE_ET_CHANGED) {
      if (e.diff > 0) {
        if (select_hist < history.size - 1)
          select_hist++;
      } else if (select_hist > 0)
        select_hist--;
    } else if (e.type == RE_ET_BTN_CLICKED) {
      hd44780_control(&lcd, true, false, false);
      hd44780_gotoxy(&lcd, 0, cursor_position);
      hd44780_puts(&lcd, "Two Clicks to Remove");
      e.type = RE_BTN_RELEASED;
      xQueueReceive(qCommand, &e, pdMS_TO_TICKS(3000));
      if (e.type == RE_ET_BTN_CLICKED) {
        remove_at_history(select_hist);
      }
      hd44780_control(&lcd, true, false, true);
    }
  }

  hd44780_control(&lcd, true, false, false);

  hd44780_clear(&lcd);
  hd44780_gotoxy(&lcd, 9, 1);
  hd44780_puts(&lcd, "no");
  hd44780_gotoxy(&lcd, 6, 2);
  hd44780_puts(&lcd, "readings");

  END_MENU_FUNCTION;
}
// Settings

uint8_t option_type_menu = 0;
switch_menu_t options_display[2] = {
    {.type_menu = &displayNormal, .loop_menu = false, .label = "Menu type: 1"},
    {.type_menu = &displayLoop, .loop_menu = true, .label = "Menu type: 2"},
};

void Change_menu(void *args) {
  option_type_menu ^= 1;
  config_menu.display = options_display[option_type_menu].type_menu;
  config_menu.loop = options_display[option_type_menu].loop_menu;
  settings_options[0].label = options_display[option_type_menu].label;

  SET_QUICK_FUNCTION;
  END_MENU_FUNCTION;
}

void Brightness(void *args) {
  hd44780_clear(&lcd);
  hd44780_puts(&lcd, "Developing");

  END_MENU_FUNCTION;
}

void Info(void *args) {
  rotary_encoder_event_t e;
  char *info_text[4] = {
      "Version: 2",
      "Developers: Marcio Bulla",
      "Repository: www.github.com/Lorxi3D/PhotogateV2",
      " ",
  };
  size_t max_scroll[4] = {0};
  size_t min_position[4] = {0};

  for (uint8_t i = 0; i < 4; i++) {
    size_t len = strlen(info_text[i]);
    max_scroll[i] = (len > 20) ? len + 1 - 20 : 0;
  }

  char text_line[21];

  while (true) {
    hd44780_clear(&lcd);

    for (uint8_t i = 0; i < 4; i++) {
      size_t position = (max_scroll[i] > 0) ? min_position[i] : 0;
      strncpy(text_line, info_text[i] + position, 20);
      text_line[20] = '\0';

      hd44780_gotoxy(&lcd, 0, i);
      hd44780_puts(&lcd, text_line);
    }

    xQueueReceive(qCommand, &e, portMAX_DELAY);

    if (e.type == RE_ET_CHANGED) {
      for (uint8_t i = 0; i < 4; i++) {
        if (e.diff > 0) {
          if (min_position[i] < max_scroll[i])
            min_position[i]++;
        } else {
          if (min_position[i] > 0)
            min_position[i]--;
        }
      }
    } else if (e.type == RE_ET_BTN_CLICKED) {
      memset(min_position, 0, sizeof(min_position));
    }
  }
}
