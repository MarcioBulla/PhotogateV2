#ifndef __MAIN_H__
#define __MAIN_H__

#include "hal/pcnt_types.h"
#include <driver/pulse_cnt.h>
#include <esp_err.h>
#include <menu_manager.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

// Start function

esp_err_t startLCD(void);

esp_err_t startEncoder(void);

esp_err_t startPCNT(void);

esp_err_t startNVS(void);

esp_err_t startPWM(void);

// Menu Manager

Navigate_t map(void);

void displayNormal(menu_path_t *current_path);

void displayLoop(menu_path_t *current_path);

extern menu_node_t root_options[5];

// Experiments

typedef struct {
  pcnt_channel_edge_action_t rising;
  pcnt_channel_edge_action_t falling;
  pcnt_glitch_filter_config_t filter;
  int32_t watchPoint[2];
} experiment_config_t;

typedef enum {
  EXPERIMENT_CONFIG = 0,
  EXPERIMENT_WAITTING,
  EXPERIMENT_TIMING,
  EXPERIMENT_DONE,
  EXPERIMENT_ERROR,
} experiment_stage_t;

typedef enum {
  ENERGY_SOLID = 0,
  ENERGY_RIRE,
  ENERGY_RE,
  ENERGY_RI,
} energy_t;

void Pendulum(void *args);

void Spring(void *args);

void Energy(void *args);

typedef struct {
  char timed[12];
  char option[8];
} experiment_data_t;

typedef struct {
  experiment_data_t *array;
  size_t size;
  size_t capability;
} experiment_data_array_t;

void append_history(experiment_data_t data);

void remove_at_history(size_t index);

void History(void *args);

// Settings

void Change_menu(void *args);

void Brightness(void *args);

void Info(void *args);

extern menu_node_t settings_options[3];

typedef struct {
  void (*type_menu)(menu_path_t *current_path);
  bool loop_menu;
} switch_menu_t;

#endif // __MAIN_H__
