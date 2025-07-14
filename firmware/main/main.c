#include "counter.h"
#include "display.h"
#include "encoder.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "memory.h"
#include "menus.h"

const static char *TAG = "main";

void app_main(void) {
  ESP_LOGI(TAG, "Starting");

  // Start flash memory of ESP
  nvs_init();
  // Initialize KY-040 (Encoder)
  encoder_init();
  // Initialize Display (LCD2004)
  display_init();
  // Initialize counter (PCNT)
  counter_init();
  // Initialize menu system
  menuInit();

  ESP_LOGI(TAG, "STARTED!!!");

  // Delete app_main task
  vTaskDelete(NULL);
}
