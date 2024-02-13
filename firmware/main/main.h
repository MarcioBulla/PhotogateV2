#ifndef __MAIN_H__
#define __MAIN_H__

#include "hal/pcnt_types.h"
#include <menu_manager.h>
#include <esp_err.h>
#include <stdint.h>
#include <driver/pulse_cnt.h>
#include <time.h>

// Start function

esp_err_t startLCD(void);

esp_err_t startEncoder(void);

esp_err_t startPCNT(void);

// Menu Manager

Navigate_t map(void);

void displayNormal(menu_path_t *current_path);

void displayLoop(menu_path_t *current_path);

extern menu_node_t root_options[4];

// Experiments 

typedef struct {
  int32_t watchPoint;
  time_t time;
}save_time_t;

typedef struct {
  pcnt_channel_edge_action_t rising;
  pcnt_channel_edge_action_t falling;
  int32_t watchPoint[2];
}experiment_config_t;

typedef enum {
  EXPERIMENT_CONFIG = 0,
  EXPERIMENT_WAITTING,
  EXPERIMENT_TIMING,
  EXPERIMENT_DONE,
}experiment_stage_t;

void crono();

void Pendulum(void *args);

void Spring(void *args);

void Energy(void *args);

// Settings

void Change_menu(void *args);

void Brightness(void *args);

void Info(void *args);

extern menu_node_t settings_options[3];

typedef struct {
  void (*type_menu)(menu_path_t *current_path);
  bool loop_menu;
  char *label;
} switch_menu_t;

#endif // __MAIN_H__
