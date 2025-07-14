#include "cronos.h"

static const char *TAG = "cronos";

/**
 * @brief Queue tha join the PCNT and Commands queues
 */
static QueueSetHandle_t qPCNT_qCommands = NULL;

static experiment_config_t *config_exp = NULL;
static uint8_t option_selected = 0;
static counter_config_t config_counter;
static time_t first_time = 0;
static time_t last_time = 0;
static time_t total_time = 0;
static hist_data_t *reading = NULL;

// Function pointer type for stage handlers
typedef experiment_stage_t (*stage_handler_t)(void);

static void print_config(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, 3);
  lcd_print(&lcd, "     !!Config!!      ");
  lcd_blink(&lcd);
  xSemaphoreGive(sDisplay);
}

static void print_waiting(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, 3);
  lcd_print(&lcd, "     !!Waiting!!     ");
  xSemaphoreGive(sDisplay);
}

static void print_timing(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, 3);
  lcd_print(&lcd, "     !!Timing!!     ");
  xSemaphoreGive(sDisplay);
}

static void print_done(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, 3);
  lcd_print(&lcd, "      !!Done!!      ");
  xSemaphoreGive(sDisplay);
}

static void print_obstruct_error(void) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 0, 3);
  lcd_print(&lcd, "!Obstructed  Sensor!");
  xSemaphoreGive(sDisplay);
}

static inline void microsecond_string(uint64_t microsecond, char *string) {
  uint32_t seconds = microsecond / 1000000U;
  uint32_t microseconds_part = microsecond - (seconds * 1000000U);
  sprintf(string, "%03lu,%06lu", seconds, microseconds_part);
}

static void update_time(time_t first, time_t last) {
  char time_string[12];

  microsecond_string(last - first, time_string);
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 5, 2);
  lcd_print(&lcd, time_string);
  xSemaphoreGive(sDisplay);
}

static void setup_experiment_display(const experiment_config_t *config_exp) {
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_clear(&lcd);
  lcd_set_cursor(&lcd, (CONFIG_DISPLAY_COLUMNS - strlen(config_exp->title)) / 2,
                 0);
  lcd_print(&lcd, config_exp->title);
  lcd_set_cursor(&lcd, 16, 2);
  lcd_print(&lcd, "s");
  xSemaphoreGive(sDisplay);
}

static void reset_experiment_stage(void) {
  xQueueReset(qPCNT);
  pcnt_unit_clear_count(pcnt_unit);
  update_time(0, 0);
  stopHourGlassAnimation(0);
  ESP_LOGI(TAG, "Experiment stage reset");
}

static bool setup_queue_set(void) {
  if (qPCNT_qCommands == NULL) {
    qPCNT_qCommands = xQueueCreateSet(10);
    xQueueAddToSet(qPCNT, qPCNT_qCommands);
    xQueueAddToSet(qCommands, qPCNT_qCommands);
    ESP_LOGI(TAG, "Queue set created and configured");
    return true;
  }
  return false;
}

static void cleanup_queue_set(void) {
  if (qPCNT_qCommands != NULL) {
    xQueueRemoveFromSet(qPCNT, qPCNT_qCommands);
    xQueueRemoveFromSet(qCommands, qPCNT_qCommands);
    vQueueDelete(qPCNT_qCommands);
    qPCNT_qCommands = NULL;
    ESP_LOGI(TAG, "Queue set cleaned up");
  }
}

static bool check_sensor_obstruction(void) {
  return gpio_get_level(CONFIG_SENSOR_IR) && config_exp->check_obstruct;
}

static void setup_counter(void) {
  xQueueReset(qPCNT);
  total_time = 0;
  pcnt_unit_clear_count(pcnt_unit);
  pcnt_config_counter(config_counter);
  startHourGlassAnimation();
  ESP_LOGI(TAG, "Waiting stage setup completed");
}

static experiment_stage_t handle_exit_stage(void) {
  ESP_LOGI(TAG, "Exiting experiment");

  // Cleanup when exiting experiment
  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_no_blink(&lcd);
  xSemaphoreGive(sDisplay);
  stopHourGlassAnimation(0);
  cleanup_queue_set();
  exitFunction();

  return EXPERIMENT_EXIT;
}

static experiment_stage_t handle_command(experiment_stage_t current_stage,
                                         const char *stage_name) {
  Navigate_t commands = NAVIGATE_NOTHING;

  if (xQueueReceive(qCommands, &commands, 0) == pdTRUE) {
    ESP_LOGI(TAG, "Command received in %s: %d", stage_name, commands);

    switch (commands) {
    case NAVIGATE_SELECT:
      ESP_LOGI(TAG, "Returning to config from %s", stage_name);
      return EXPERIMENT_CONFIG;

    case NAVIGATE_BACK:
      ESP_LOGI(TAG, "Exiting experiment from %s", stage_name);
      return EXPERIMENT_EXIT;

    default:
      ESP_LOGW(TAG, "Unknown command in %s: %d", stage_name, commands);
      break;
    }
  }

  return current_stage;
}

static experiment_stage_t handle_config_stage(void) {
  Navigate_t commands = NAVIGATE_NOTHING;
  QueueHandle_t qActive = NULL;
  experiment_stage_t current_stage = EXPERIMENT_CONFIG;

  reset_experiment_stage();
  print_config();

  while (current_stage == EXPERIMENT_CONFIG) {
    (*config_exp->print_option)(option_selected);

    do {
      qActive = xQueueSelectFromSet(qPCNT_qCommands, portMAX_DELAY);
    } while (qActive != qCommands);

    xQueueReceive(qCommands, &commands, 0);

    switch (commands) {
    case NAVIGATE_UP:
      if (option_selected < config_exp->interval_options[1]) {
        option_selected++;
      }
      break;

    case NAVIGATE_DOWN:
      if (option_selected > config_exp->interval_options[0]) {
        option_selected--;
      }
      break;

    case NAVIGATE_SELECT:
      config_counter = (*config_exp->mk_counter_config)(option_selected);
      xSemaphoreTake(sDisplay, portMAX_DELAY);
      lcd_no_blink(&lcd);
      xSemaphoreGive(sDisplay);

      ESP_LOGI(TAG, "Config selected, option: %d", option_selected);

      // Check for sensor obstruction before proceeding to waiting
      if (check_sensor_obstruction()) {
        ESP_LOGI(TAG, "Sensor obstructed, entering error stage");
        current_stage = EXPERIMENT_ERROR;
      } else {
        // Setup counter and start hourglass animation
        setup_counter();
        current_stage = EXPERIMENT_WAITTING;
      }
      break;

    case NAVIGATE_BACK:
      current_stage = EXPERIMENT_EXIT;
      break;

    default:
      ESP_LOGW(TAG, "Invalid command in CONFIG: %d", commands);
      break;
    }

    ESP_LOGI(TAG, "Option: %d", option_selected);
  }

  return current_stage;
}

static experiment_stage_t handle_error_stage(void) {
  QueueHandle_t qActive = NULL;
  experiment_stage_t current_stage = EXPERIMENT_ERROR;

  print_obstruct_error();

  while (current_stage == EXPERIMENT_ERROR) {
    for (uint8_t i = 0; i < 5; i++) {
      qActive = xQueueSelectFromSet(qPCNT_qCommands, pdMS_TO_TICKS(100));

      if (qActive == qCommands) {
        return handle_command(current_stage, "ERROR");
      }

      if (gpio_get_level(CONFIG_SENSOR_IR)) {
        i = 0; // Reset counter if still obstructed
      } else {
        ESP_LOGI(TAG, "Sensor cleared");
        current_stage = EXPERIMENT_WAITTING;
      }
    }
  }

  if (current_stage == EXPERIMENT_WAITTING) {
    setup_counter();
  }

  return current_stage;
}

static experiment_stage_t handle_waiting_stage(void) {
  QueueHandle_t qActive = NULL;
  experiment_stage_t current_stage = EXPERIMENT_WAITTING;

  print_waiting();

  while (current_stage == EXPERIMENT_WAITTING) {
    qActive = xQueueSelectFromSet(qPCNT_qCommands, portMAX_DELAY);

    if (qActive == qPCNT) {
      if (xQueueReceive(qPCNT, &first_time, 0) == pdTRUE) {
        ESP_LOGI(TAG, "PCNT event received, starting timing");
        current_stage = EXPERIMENT_TIMING;
      }
    } else if (qActive == qCommands) {
      return handle_command(current_stage, "WAITTING");
    }
  }

  return current_stage;
}

static experiment_stage_t handle_timing_stage(void) {
  QueueHandle_t qActive = NULL;
  experiment_stage_t current_stage = EXPERIMENT_TIMING;

  print_timing();

  while (current_stage == EXPERIMENT_TIMING) {
    // Update display with current time
    last_time = esp_timer_get_time() + esp_random() % 10000;
    update_time(first_time - total_time, last_time);
    (*config_exp->print_option)(option_selected);

    qActive = xQueueSelectFromSet(qPCNT_qCommands, pdMS_TO_TICKS(40));

    if (qActive == qPCNT) {
      if (xQueueReceive(qPCNT, &last_time, 0) == pdTRUE) {
        ESP_LOGI(TAG, "Timing completed");
        if (config_counter.resetCounter) {
          current_stage = EXPERIMENT_WAITTING;
          total_time += last_time - first_time;
        } else {
          current_stage = EXPERIMENT_DONE;
        }
        update_time(first_time - total_time, last_time);
      }
    } else if (qActive == qCommands) {
      return handle_command(current_stage, "TIMING");
    }
  }

  return current_stage;
}

static experiment_stage_t handle_done_stage(void) {
  QueueHandle_t qActive = NULL;
  experiment_stage_t current_stage = EXPERIMENT_DONE;

  print_done();
  stopHourGlassAnimation(3);

  // Save reading to history
  microsecond_string(last_time - first_time, reading->time);
  reading->type = (*config_exp->print_option)(option_selected);
  add_reading(*reading);

  while (current_stage == EXPERIMENT_DONE) {
    qActive = xQueueSelectFromSet(qPCNT_qCommands, portMAX_DELAY);

    if (qActive == qCommands) {
      return handle_command(current_stage, "DONE");
    }
  }

  return current_stage;
}

static const stage_handler_t stage_handlers[] = {
    [EXPERIMENT_CONFIG] = handle_config_stage,
    [EXPERIMENT_WAITTING] = handle_waiting_stage,
    [EXPERIMENT_TIMING] = handle_timing_stage,
    [EXPERIMENT_DONE] = handle_done_stage,
    [EXPERIMENT_ERROR] = handle_error_stage,
    [EXPERIMENT_EXIT] = handle_exit_stage,
};

/**
 * @brief Size of stage_handlers
 */
static const size_t num_stage_handlers =
    sizeof(stage_handlers) / sizeof(stage_handlers[0]);

void run_experiment(experiment_config_t config) {
  experiment_stage_t stage = EXPERIMENT_CONFIG;

  // Setup queue set
  setup_queue_set();

  // Initialize global variables
  config_exp = &config;
  option_selected = config.default_value;
  first_time = 0;
  last_time = 0;

  // Prepare history data structure
  char time[12];
  char type[6];
  hist_data_t history = {
      .time = time,
      .type = type,
  };
  reading = &history;

  // Setup display
  setup_experiment_display(config_exp);

  // Main experiment loop with jump table
  while (true) {

    ESP_LOGI(TAG, "Current stage: %d", stage);

    // Validate stage index and call handler via jump table
    if (stage >= 0 && stage < num_stage_handlers &&
        stage_handlers[stage] != NULL) {
      stage = stage_handlers[stage]();
    } else {
      ESP_LOGW(TAG, "Invalid or unimplemented stage: %d", stage);
      stage = EXPERIMENT_CONFIG; // Default fallback
    }
  }

  ESP_LOGI(TAG, "Experiment ended");
}
