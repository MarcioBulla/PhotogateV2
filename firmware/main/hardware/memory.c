#include "memory.h"

static const char *TAG = "NVS";

nvs_handle_t nvsStorage;

esp_err_t nvs_init(void) {

  esp_err_t ret;

  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_LOGI(TAG, "nvs flash init: %s", esp_err_to_name(ret));

  return ret;
}
