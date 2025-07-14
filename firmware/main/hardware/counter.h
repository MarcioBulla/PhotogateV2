#pragma once

#include "driver/pulse_cnt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "stdint.h"

extern pcnt_unit_handle_t pcnt_unit;
extern pcnt_channel_handle_t pcnt_channel;
extern QueueHandle_t qPCNT;

typedef struct {
  pcnt_channel_edge_action_t rising;
  pcnt_channel_edge_action_t falling;
  pcnt_glitch_filter_config_t filter;
  int32_t watchPoint[2];
  BaseType_t (*resetCounter)(const pcnt_watch_event_data_t *edata,
                             void *user_ctx, int64_t time_now);
} counter_config_t;

esp_err_t counter_init(void);

void pcnt_config_counter(counter_config_t config_counter);
