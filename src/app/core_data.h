#ifndef CORE_DATA_H
#define CORE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define CORE_PATH_MAX   256

typedef struct core_data_t
{
  bool style_act;                     // use syntax highlighting
  char exec_path[CORE_PATH_MAX];      // path to executable
  char sheets_path[CORE_PATH_MAX];    // path to sheets folder

}core_data_t;
#define CORE_DATA_INIT()    \
{                           \
  .style_act = true,        \
}

core_data_t* core_data_get();

#ifdef __cplusplus
}   // extern c
#endif

#endif
