#ifndef CORE_DATA_H
#define CORE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define CORE_PATH_MAX   256
#define CORE_SHEET_PATHS_MAX 8

typedef struct core_data_t
{
  char exec_path[CORE_PATH_MAX];      // path to executable
  char config_path[CORE_PATH_MAX];    // path to config file
  // set in config.doc now
  // char sheets_path[CORE_PATH_MAX];    // path to sheets folder

  bool style_act;                     // use syntax highlighting
  bool style_deact_cmd;               // -c/-color command
  // bool builtin_sheets_act;            // [builtin_sheets] in config
  bool print_loc_act;                 // -loc command

  char sheet_paths[CORE_SHEET_PATHS_MAX][CORE_PATH_MAX];
  int  sheet_paths_len;

}core_data_t;
#define CORE_DATA_INIT()            \
{                                   \
  .style_act              = true,   \
  .style_deact_cmd        = false,  \
  .print_loc_act          = false,  \
  .sheet_paths_len        = 0,      \
}

core_data_t* core_data_get();

void core_data_init_config_path();

void core_data_init_sheet_path(); // not used

#ifdef __cplusplus
}   // extern c
#endif

#endif
