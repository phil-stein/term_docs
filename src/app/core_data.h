#ifndef CORE_DATA_H
#define CORE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define CORE_PATH_MAX   256
#define CORE_CUSTOM_SHEETS_MAX 8

typedef struct core_data_t
{
  bool style_act;                     // use syntax highlighting
  bool style_deact_cmd;               // -c/-color command
  char exec_path[CORE_PATH_MAX];      // path to executable
  char sheets_path[CORE_PATH_MAX];    // path to sheets folder

  char custom_sheet_paths[CORE_CUSTOM_SHEETS_MAX][CORE_PATH_MAX];
  int  custom_sheet_paths_len;

}core_data_t;
#define CORE_DATA_INIT()        \
{                               \
  .style_act       = true,      \
  .style_deact_cmd = false,     \
  .custom_sheet_paths_len = 0,  \
}

core_data_t* core_data_get();

#ifdef __cplusplus
}   // extern c
#endif

#endif
