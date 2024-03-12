#include "app/core_data.h"

core_data_t  core_data_data = CORE_DATA_INIT();
core_data_t* core_data = &core_data_data;

// core_data_t* core_data_get()
// {
//   return &core_data;
// }

void core_data_init_config_path()
{
  strncpy(core_data->config_path, core_data->exec_path, CORE_PATH_MAX);
  ERR_CHECK(core_data->config_path != NULL, "failed copying executable path\n");
  int dirs_walk_back_02 = 1 + DIRS_TO_WALK_BACK_TO_ROOT;
  for (u32 i = strlen(core_data->config_path) -1; i > 0; --i)
  {
    if (core_data->config_path[i] == '\\' || 
        core_data->config_path[i] == '/')
    { dirs_walk_back_02--; if (dirs_walk_back_02 <= 0) { break; } }
    core_data->config_path[i] = '\0';
  }
  // replace '\' with '/'
  for (u32 i = 0; i < strlen(core_data->config_path); ++i)
  {
      if (core_data->config_path[i] == '\\') 
      { core_data->config_path[i] = '/'; }
  }
  strcat(core_data->config_path, "config.doc");
}

void core_data_init_sheet_path()
{
  // // -- get the sheet path either based on executable or macro --
  // 
  // strncpy(core_data->sheets_path, core_data->exec_path, CORE_PATH_MAX);
  // assert(core_data->sheets_path != NULL);
  // int dirs_walk_back = 1 + DIRS_TO_WALK_BACK_TO_ROOT;
  // for (u32 i = strlen(core_data->sheets_path) -1; i > 0; --i)
  // {
  //   if (core_data->sheets_path[i] == '\\' || core_data->sheets_path[i] == '/') 
  //   { dirs_walk_back--; if (dirs_walk_back <= 0) { break; } }
  //   core_data->sheets_path[i] = '\0';
  // }
  // // replace '\' with '/'
  // for (u32 i = 0; i < strlen(core_data->sheets_path); ++i)
  // {
  //     if (core_data->sheets_path[i] == '\\') { core_data->sheets_path[i] = '/'; }
  // }
  // strcat(core_data->sheets_path, "sheets/builtin_sheets/");
}
