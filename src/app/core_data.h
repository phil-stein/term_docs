#ifndef CORE_DATA_H
#define CORE_DATA_H

#include "global/global.h"

typedef struct core_data_t
{
  bool style_act;    // use syntax highlighting

}core_data_t;
#define CORE_DATA_INIT()    \
{                           \
  .style_act = true,        \
}

core_data_t* core_data_get();

#endif
