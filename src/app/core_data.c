#include "app/core_data.h"

core_data_t core_data = CORE_DATA_INIT();

core_data_t* core_data_get()
{
  return &core_data;
}
