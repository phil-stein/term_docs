#include "app/config.h"
#include "app/file_io.h"
#include "app/core_data.h"

#include <ctype.h>


#define BUF_MAX 256
char name_buf[BUF_MAX];
int  name_buf_pos = 0;
char value_buf[BUF_MAX];
int  value_buf_pos = 0;


void config_read_config_file(const char* path)
{
  if (!check_file_exists(path)) { P_ERR("couldnt find config file\n"); return; }
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);
  

  for (int c = 0; c < txt_len; ++c)
  {
    // start of argument name
    if (txt[c] == '[')
    {
      // reset here so these are accesible inside other funcs, f.e. config_handle_argument()
      name_buf_pos = 0;
      value_buf_pos = 0;
      
      // -- read argument name --
      
      c++;  // skip '['
      while (c < txt_len && 
             name_buf_pos < BUF_MAX -1 && 
             txt[c] != ']')
      { name_buf[name_buf_pos++] = txt[c++]; }
      name_buf[name_buf_pos] = '\0';
      c++; // skip ']'

      // -- read argument value --

      // skip whitespace
      while (c < txt_len && isspace(txt[c])) { c++; }

      while (c < txt_len && 
             (
              isalnum(txt[c])  || 
              txt[c] == '/'    ||
              txt[c] == '\\'   ||
              txt[c] == '-'   ||
              txt[c] == '_'   ||
              txt[c] == '.'    
             )
            ) 
      { value_buf[value_buf_pos++] = txt[c++]; }
      value_buf[value_buf_pos] = '\0';
      
      config_handle_argument();
    }
  }
}

void config_handle_argument()
{
  core_data_t* core_data = core_data_get();

  // P_STR(name_buf);
  // P_STR(value_buf);

  // [syntax]
  if (name_buf[0] == 's' &&
      name_buf[1] == 'y' &&
      name_buf[2] == 'n' &&
      name_buf[3] == 't' &&
      name_buf[4] == 'a' &&
      name_buf[5] == 'x' )
  {

    int value = config_parse_bool();
    if (!(value == 0 || value == 1))
    { P_ERR("incorrect argument format for [syntax]\n"); return; }

    // PF("syntax: %d\n", value);
    core_data->style_act = (bool)value;
  }

  // [sheet_path]
  if (name_buf[0] == 's' &&
      name_buf[1] == 'h' &&
      name_buf[2] == 'e' &&
      name_buf[3] == 'e' &&
      name_buf[4] == 't' &&
      name_buf[5] == '_' &&
      name_buf[6] == 'd' &&
      name_buf[7] == 'i' &&
      name_buf[8] == 'r' )
  {
    // get path relative to executable
    char path[CORE_PATH_MAX];
    strncpy(path, core_data->exec_path, CORE_PATH_MAX);
    assert(path != NULL);
    int dirs_walk_back = 2;
    for (u32 i = strlen(path) -1; i > 0; --i)
    {
      if (path[i] == '\\') 
      { dirs_walk_back--; if (dirs_walk_back <= 0) { break; } }
      path[i] = '\0';
    }
    strcat(path, value_buf);
    strcat(path, "\\");
    // PF("sheet_path: %s\n", path);
    if (!check_dir_exists(path)) { P_ERR("couldnt find path given for [sheet_dir]: %s\n", value_buf); return; }
   
    if (core_data->custom_sheet_paths_len < CORE_CUSTOM_SHEETS_MAX)
    {
      strncpy(core_data->custom_sheet_paths[core_data->custom_sheet_paths_len], path, strlen(path));
      // P_STR(core_data->custom_sheet_paths[core_data->custom_sheet_paths_len]);
      core_data->custom_sheet_paths_len++;
    }
    else { P_ERR("cant add additional [sheet_dir] paths, max is %d\n", CORE_CUSTOM_SHEETS_MAX); }
  }

}

int config_parse_bool()
{
  if (value_buf[0] == '0' &&
      value_buf[1] == '\0')
  { return 0; }
  if (value_buf[0] == '1' &&
      value_buf[1] == '\0')
  { return 1; }

  if (value_buf[0] == 'f' &&
      value_buf[1] == 'a' &&
      value_buf[2] == 'l' &&
      value_buf[3] == 's' &&
      value_buf[4] == 'e' &&
      value_buf[5] == '\0')
  { return 0; }
  if (value_buf[0] == 't' &&
      value_buf[1] == 'r' &&
      value_buf[2] == 'u' &&
      value_buf[3] == 'e' &&
      value_buf[4] == '\0')
  { return 1; }

  return -1;  // incorrect formatting
}
