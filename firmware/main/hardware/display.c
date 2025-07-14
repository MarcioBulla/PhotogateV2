#include "display.h"

const static char *TAG = "display";

i2c_lcd_pcf8574_handle_t lcd;

// Semaphore to control the display
SemaphoreHandle_t sDisplay;

// Variable to store the current brightness
uint8_t Brightness;

// Title of function on menu
char brightnessLabel[16];

// Custom characters to display
static uint8_t char_data[] = {
    //  0 - load
    0x18, 0x1c, 0x10, 0x1c, 0x1e, 0x18, 0x1e, 0x18,
    // 1 - char  subscript e
    0x00, 0x00, 0x00, 0x0c, 0x12, 0x1c, 0x10, 0x0e,
    // 2 - char subscript i
    0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x0c,
    // 3 - number symbol
    0x0c, 0x12, 0x12, 0x0c, 0x00, 0x1e, 0x00, 0x00,
    // 4 - hourglass 1
    0x1f, 0x1f, 0x0e, 0x04, 0x04, 0x0a, 0x11, 0x1f,
    // 5 - hourglass 2
    0x1f, 0x1b, 0x0e, 0x04, 0x04, 0x0e, 0x11, 0x1f,
    // 6 - hourglass 3
    0x1f, 0x11, 0x0e, 0x04, 0x04, 0x0e, 0x1b, 0x1f,
    // 7 - hourglass 4
    0x1f, 0x11, 0x0a, 0x04, 0x04, 0x0e, 0x1f, 0x1f};

static i2c_config_t i2cConfig = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = CONFIG_I2C_SDA,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = CONFIG_I2C_SCL,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 100000,
};

static ledc_timer_config_t backlightTimer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1000,
    .duty_resolution = LEDC_TIMER_13_BIT,
    .clk_cfg = LEDC_AUTO_CLK,
};

static ledc_channel_config_t backlightChannel = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .gpio_num = CONFIG_PWM_DISPLAY,
    .duty = 8191,
    .hpoint = 0,
};

esp_err_t displaySetBrightness(uint32_t brightness) {
  esp_err_t ret = ESP_OK;
  ledc_set_duty(backlightChannel.speed_mode, backlightChannel.channel,
                brightness);
  ledc_update_duty(backlightChannel.speed_mode, backlightChannel.channel);
  return ret;
}

/**
 * @brief Print bar of brightness
 *
 * @param level Value [0,100] brightness level
 */
static void print_bar(uint8_t level) {
  char percent[5];
  char bar[21];
  uint8_t integer = level / 5;

  memset(bar, '\xFF', integer);
  if (level % 5 > 2) {
    bar[integer] = '\x08';
    integer++;
  }
  memset(bar + integer, ' ', 20 - integer);
  bar[20] = '\0';

  snprintf(percent, sizeof(percent), "%03d%%", level);

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_set_cursor(&lcd, 8, 1);
  lcd_print(&lcd, percent);
  lcd_set_cursor(&lcd, 0, 2);
  lcd_print(&lcd, bar);
  lcd_set_cursor(&lcd, 7, 1);
  xSemaphoreGive(sDisplay);
}

void fMenu_brightnessControl(void *args) {
  Navigate_t command = NAVIGATE_NOTHING;

  uint8_t brightnessTemp = Brightness;

  xSemaphoreTake(sDisplay, portMAX_DELAY);
  lcd_clear(&lcd);
  lcd_set_cursor(&lcd, 3, 0);
  lcd_print(&lcd, "Set Brightness");
  lcd_set_cursor(&lcd, 3, 3);
  lcd_print(&lcd, "Click To Save!");
  lcd_blink(&lcd);
  xSemaphoreGive(sDisplay);

  while (true) {
    print_bar(brightnessTemp);
    xQueueReceive(qCommands, &command, portMAX_DELAY);

    switch (command) {
    case NAVIGATE_UP:
      if (brightnessTemp < 100) {
        brightnessTemp++;
      }
      break;
    case NAVIGATE_DOWN:
      if (brightnessTemp > 0) {
        brightnessTemp--;
      }
      break;
    case NAVIGATE_SELECT:
      Brightness = brightnessTemp;
      // Save value on Non Volatile Storage
      esp_err_t ret = ESP_OK;
      ret = nvs_open("storage", NVS_READWRITE, &nvsStorage);
      if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
        exitFunction();
      } else {
        ESP_LOGI(TAG, "Brightness set to %03d%%", Brightness);
        nvs_set_u8(nvsStorage, "brightness", Brightness);
        nvs_commit(nvsStorage);
      }
      nvs_close(nvsStorage);
      // Rename brightness menu function label
      snprintf(brightnessLabel, 16, "Brightness %03d%%", Brightness);
      lcd_no_blink(&lcd);
      exitFunction();
      break;
    case NAVIGATE_BACK:
      ESP_LOGI(TAG, "Cancel");
      displaySetBrightness(PERCENT_TO_13_BIT_CUSTOM(Brightness));
      lcd_no_blink(&lcd);
      exitFunction();
      break;
    default:
      ESP_LOGW(TAG, "Indefined command");
      break;
    }
    displaySetBrightness(PERCENT_TO_13_BIT_CUSTOM(brightnessTemp));
  }
}

esp_err_t display_init(void) {
  ESP_LOGI(TAG, "Creating display's semaphore");
  vSemaphoreCreateBinary(sDisplay);
  xSemaphoreGive(sDisplay);

  ESP_LOGI(TAG, "Starting I2C");
  ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2cConfig));
  ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
  ESP_LOGI(TAG, "I2C STARTED");

  ESP_LOGI(TAG, "Initializing display's pwm");
  ESP_ERROR_CHECK(ledc_timer_config(&backlightTimer));
  ESP_ERROR_CHECK(ledc_channel_config(&backlightChannel));
  ESP_LOGI(TAG, "Display's pwm started");

  ESP_LOGI(TAG, "Initializing display");
  lcd_init(&lcd, CONFIG_DISPLAY_ADDR, I2C_NUM_0);
  ESP_LOGI(TAG, "SET SIZE");
  lcd_begin(&lcd, CONFIG_DISPLAY_COLUMNS, CONFIG_DISPLAY_ROWS);

  ESP_LOGI(TAG, "SET CHAR DATA");
  for (uint8_t _ = 0; _ < 8; _++) {
    lcd_create_char(&lcd, _, char_data + (_ * 8));
  }

  lcd_set_backlight(&lcd, 255);

  // Get brightness saved
  esp_err_t ret = ESP_OK;
  ret = nvs_open("storage", NVS_READWRITE, &nvsStorage);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "error (%s) opening nvs handle!", esp_err_to_name(ret));
  } else {

    // get brightness
    ret = nvs_get_u8(nvsStorage, "brightness", &Brightness);
    switch (ret) {

    case ESP_OK:
      ESP_LOGI(TAG, "read brightness = %d ", Brightness);
      break;

    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGW(TAG, "the brightness value is not initialized yet!");
      Brightness = 100;
      nvs_set_u8(nvsStorage, "brightness", Brightness);
      nvs_commit(nvsStorage);
      break;

    default:
      ESP_LOGE(TAG, "error (%s) reading home!", esp_err_to_name(ret));
      return ret;
    }
  }
  nvs_close(nvsStorage);

  displaySetBrightness(PERCENT_TO_13_BIT_CUSTOM(Brightness));
  snprintf(brightnessLabel, 16, "Brightness %03d%%", Brightness);

  ESP_LOGI(TAG, "Display started");

  return ESP_OK;
}
