#include "app/config.h"
#include "app/file_io.h"
#include "app/core_data.h"
#include "app/style.h"
#include "app/doc.h"

#include <ctype.h>

#include <Windows.h>  // SetConsoleOutputCP(CP_UTF8)

#define BUF_MAX 256
char name_buf[BUF_MAX];
int  name_buf_pos = 0;
char value_buf[BUF_MAX];
int  value_buf_pos = 0;

int   txt_len = 0;
char* txt = NULL;
int   c = 0;  // current char in txt[c]

void config_read_config_file(const char* path, bool print_config)
{
  // core_data_t* core_data = core_data_get();

  if (!check_file_exists(path)) { _P_ERR("couldnt find config file\n"); return; }
  txt = read_text_file_len(path, &txt_len);

  for (c = 0; c < txt_len; ++c)
  {
    // skip comments
    if (txt[c] == '/' && txt[c +1] == '/')
    { 
      c += 2; 
      while (c < txt_len && (txt[c] != '\n' && txt[c] != '\0')) { c++; } 
    }
    
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

      // value is string
      if (txt[c] == '"')
      {
        // c++;  // skip '"'
        value_buf[value_buf_pos++] = txt[c++]; 
        while ( c < txt_len && 
               ( txt[c] != '"' || (txt[c] == '"' && txt[c-1] == '\\') ) )
        { value_buf[value_buf_pos++] = txt[c++]; }
        value_buf[value_buf_pos++] = txt[c++]; 
      }
      else  // value isnt a string
      {
        while (c < txt_len && !isspace(txt[c]) ) 
        { 
          value_buf[value_buf_pos++] = txt[c++]; 
        }
      }
      value_buf[value_buf_pos] = '\0';
      // P_STR(value_buf);
      argument_t argument = ARGUMENT_T_INIT();
      config_error_flag err = config_parse_argument(&argument, value_buf, value_buf_pos);
      if (err != CONFIG_ERROR_NO_ERROR) 
      { 
        _P_ERR("parsing arg: |%s|\n", value_buf); 
        P_CONFIG_ERROR_FLAG(err);
      }
      // else
      // {
      //   switch (argument.type)
      //   { 
      //     case ARGUMENT_TYPE_INVALID:
      //       ERR("arg invalid: |%s|\n", value_buf);
      //       break;
      //     case ARGUMENT_TYPE_STR:
      //       P_STR(value_buf);
      //       char* _c = argument._str + argument._str_len;
      //       char   x = *_c;
      //       *_c = '\0';
      //       P_STR(argument._str);
      //       *_c = x;
      //       break;
      //     case ARGUMENT_TYPE_BOOL:
      //       P_STR(value_buf);
      //       P_BOOL(argument._bool);
      //       break;
      //     case ARGUMENT_TYPE_INT:
      //       P_STR(value_buf);
      //       P_INT(argument._int);
      //       break;
      //     case ARGUMENT_TYPE_F32:
      //       P_STR(value_buf);
      //       P_F32(argument._f32);
      //       break;
      //   }
      // }

      config_handle_argument(&argument);
    }
  }

  // -c/-color command overrides config settings
  if (core_data->style_deact_cmd) { core_data->style_act = false; }
  // set console to use utf8, windows-specific
  if (core_data->use_utf8)
  { SetConsoleOutputCP(CP_UTF8); }
  
  if (print_config)
  {
    // DOC_PF_COLOR(PF_PURPLE);
    // P_LINE_STR("config.doc ");
    // DOC_PF_COLOR(PF_WHITE);
    style_draw_title(" config.doc ");
    
    PF("%s\n", txt);
    
    DOC_PF_COLOR(PF_PURPLE);
    
    DOC_PF_STYLE(PF_DIM, PF_WHITE);
    P_LINE();
    DOC_PF_STYLE_RESET();

    DOC_PF_COLOR(PF_CYAN);
    PF("config path");
    DOC_PF_COLOR(PF_WHITE);
    PF(": %s\n", path);
     
    DOC_PF_COLOR(PF_CYAN); PF("syntax"); DOC_PF_COLOR(PF_WHITE);
    PF(": %s\n", STR_BOOL(core_data->style_act));
    
    DOC_PF_COLOR(PF_CYAN); PF("location"); DOC_PF_COLOR(PF_WHITE);
    PF(": %s\n", STR_BOOL(core_data->print_loc_act));
    
    if (!core_data->use_utf8 && core_data->use_icons)
    { 
      DOC_PF_COLOR(PF_RED); PF("[ERROR]"); DOC_PF_COLOR(PF_WHITE); 
      PF(" cant activate [icons] without also setting [utf8]\n");
      core_data->use_icons = false;
    }
    DOC_PF_COLOR(PF_CYAN); PF("utf8"); DOC_PF_COLOR(PF_WHITE);
    PF(": %s\n", STR_BOOL(core_data->use_utf8));
    DOC_PF_COLOR(PF_CYAN); PF("icons"); DOC_PF_COLOR(PF_WHITE);
    PF(": %s\n", STR_BOOL(core_data->use_icons));
    
    DOC_PF_COLOR(PF_YELLOW); PF("max icon name len"); DOC_PF_COLOR(PF_WHITE);
    PF(": %d\n", CORE_ICONS_MAX);
    DOC_PF_COLOR(PF_CYAN); PF("error_icon"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->error_icon);
    DOC_PF_COLOR(PF_CYAN); PF("warning_icon"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->warning_icon);
    DOC_PF_COLOR(PF_CYAN); PF("info_icon"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->info_icon);
    DOC_PF_COLOR(PF_CYAN); PF("link_icon"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->link_icon);
    
    DOC_PF_COLOR(PF_CYAN); PF("seperator_left"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->seperator_left);
    DOC_PF_COLOR(PF_CYAN); PF("seperator_right"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->seperator_right);
    DOC_PF_COLOR(PF_CYAN); PF("border"); DOC_PF_COLOR(PF_WHITE);
    PF(": '%s'\n", core_data->border);

    DOC_PF_COLOR(PF_YELLOW); PF("max sheet paths"); DOC_PF_COLOR(PF_WHITE);
    PF(": %d\n", CORE_SHEET_PATHS_MAX);

    for (int i = 0; i < core_data->sheet_paths_len; ++i)
    {
      int lines = 0;
      doc_count_lines_dir(core_data->sheet_paths[i], &lines);
      
      DOC_PF_COLOR(PF_CYAN); PF("path[%d]", i); DOC_PF_COLOR(PF_WHITE);
      PF(": %s\n", core_data->sheet_paths[i]);
      PF(" -> lines: %d\n", lines);
    }
    
    // DOC_PF_COLOR(PF_PURPLE);
    // P_LINE();
    // DOC_PF_COLOR(PF_WHITE);
    style_draw_line();
  }

  FREE(txt);
  txt_len = 0;
  c = 0;
}

void config_handle_argument(argument_t* arg)
{
  // core_data_t* core_data = core_data_get();

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
    if (arg->type != ARGUMENT_TYPE_BOOL)
    { _P_ERR("incorrect argument format for [syntax]\n -> 'true', 'false', '1', '0'\n"); return; }
    core_data->style_act = arg->_bool;
  }
  // [location]
  else if (name_buf[0] == 'l' &&
           name_buf[1] == 'o' &&
           name_buf[2] == 'c' &&
           name_buf[3] == 'a' &&
           name_buf[4] == 't' &&
           name_buf[5] == 'i' &&
           name_buf[6] == 'o' &&
           name_buf[7] == 'n' )
  {
    if (arg->type != ARGUMENT_TYPE_BOOL)
    { _P_ERR("incorrect argument format for [location]\n -> 'true', 'false', '1', '0'\n"); return; }
    core_data->print_loc_act = arg->_bool;
  }

  // [utf8]
  else if (name_buf[0] == 'u' &&
           name_buf[1] == 't' &&
           name_buf[2] == 'f' &&
           name_buf[3] == '8' )
  {
    if (arg->type != ARGUMENT_TYPE_BOOL)
    { _P_ERR("incorrect argument format for [utf8]\n -> 'true', 'false', '1', '0'\n"); return; }
    core_data->use_utf8 = arg->_bool;
  }
  // [icons]
  else if (name_buf[0] == 'i' &&
           name_buf[1] == 'c' &&
           name_buf[2] == 'o' &&
           name_buf[3] == 'n' &&
           name_buf[4] == 's' )
  {
    if (arg->type != ARGUMENT_TYPE_BOOL)
    { _P_ERR("incorrect argument format for [icons]\n -> 'true', 'false', '1', '0'\n"); return; }
    core_data->use_icons = arg->_bool;
  }
  // [error_icon]
  else if (name_buf[0] == 'e' &&
           name_buf[1] == 'r' &&
           name_buf[2] == 'r' &&
           name_buf[3] == 'o' &&
           name_buf[4] == 'r' &&
           name_buf[5] == '_' &&
           name_buf[6] == 'i' &&
           name_buf[7] == 'c' &&
           name_buf[8] == 'o' &&
           name_buf[9] == 'n' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [error_icon]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [error_icon] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->error_icon, arg->_str, arg->_str_len); }
  }
  // [warning_icon]
  else if (name_buf[ 0] == 'w' &&
           name_buf[ 1] == 'a' &&
           name_buf[ 2] == 'r' &&
           name_buf[ 3] == 'n' &&
           name_buf[ 4] == 'i' &&
           name_buf[ 5] == 'n' &&
           name_buf[ 6] == 'g' &&
           name_buf[ 7] == '_' &&
           name_buf[ 8] == 'i' &&
           name_buf[ 9] == 'c' &&
           name_buf[10] == 'o' &&
           name_buf[11] == 'n' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [warning_icon]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [warning_icon] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->warning_icon, arg->_str, arg->_str_len); }
  }
  // [info_icon]
  else if (name_buf[0] == 'i' &&
           name_buf[1] == 'n' &&
           name_buf[2] == 'f' &&
           name_buf[3] == 'o' &&
           name_buf[4] == '_' &&
           name_buf[5] == 'i' &&
           name_buf[6] == 'c' &&
           name_buf[7] == 'o' &&
           name_buf[8] == 'n' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [info_icon]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [info_icon] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->info_icon, arg->_str, arg->_str_len); }
  }
  // [link_icon]
  else if (name_buf[0] == 'l' &&
           name_buf[1] == 'i' &&
           name_buf[2] == 'n' &&
           name_buf[3] == 'k' &&
           name_buf[4] == '_' &&
           name_buf[5] == 'i' &&
           name_buf[6] == 'c' &&
           name_buf[7] == 'o' &&
           name_buf[8] == 'n' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [link_icon]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [link_icon] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->link_icon, arg->_str, arg->_str_len); }
  }

  // [sheet_dir_rel]
  else if (name_buf[ 0] == 's' &&
           name_buf[ 1] == 'h' &&
           name_buf[ 2] == 'e' &&
           name_buf[ 3] == 'e' &&
           name_buf[ 4] == 't' &&
           name_buf[ 5] == '_' &&
           name_buf[ 6] == 'd' &&
           name_buf[ 7] == 'i' &&
           name_buf[ 8] == 'r' &&
           name_buf[ 9] == '_' &&
           name_buf[10] == 'r' &&
           name_buf[11] == 'e' &&
           name_buf[12] == 'l' )
  {    
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [sheet_dir_rel]\n -> string: \"...\"\n"); return; }
    // get path relative to executable
    char path[CORE_PATH_MAX];
    STRNCPY(path, core_data->exec_path, CORE_PATH_MAX);
    int dirs_walk_back = 1 + DIRS_TO_WALK_BACK_TO_ROOT;
    for (u32 i = strlen(path) -1; i > 0; --i)
    {
      if (path[i] == '\\' || path[i] == '/')
      { dirs_walk_back--; if (dirs_walk_back <= 0) { break; } }
      path[i] = '\0';
    }
    // strcat(path, value_buf);
    strncat(path, arg->_str, arg->_str_len);
    int path_len = (int)strlen(path);
    if (path[path_len-1] != '/' &&
        path[path_len-1] != '\\')
    { strcat(path, "/"); path_len++; }
    // replace '\' with '/'
    for (u32 i = 0; i < strlen(path); ++i)
    { if (path[i] == '\\') { path[i] = '/'; } }
    // PF("sheet_path: %s\n", path);
    
    if (!check_dir_exists(path)) { _P_ERR("couldnt find path given for [sheet_dir_rel]: %s\n -> %s\n", value_buf, path); return; }
   
    if (core_data->sheet_paths_len < CORE_SHEET_PATHS_MAX)
    {
      strncpy(core_data->sheet_paths[core_data->sheet_paths_len], path, strlen(path));
      // P_STR(core_data->sheet_paths[core_data->sheet_paths_len]);
      core_data->sheet_paths_len++;
    }
    else { _P_ERR("cant add additional [sheet_dir] / [sheet_dir_rel] paths, max is %d\n", CORE_SHEET_PATHS_MAX); }
  }
  // [sheet_dir]
  else if (name_buf[0] == 's' &&
           name_buf[1] == 'h' &&
           name_buf[2] == 'e' &&
           name_buf[3] == 'e' &&
           name_buf[4] == 't' &&
           name_buf[5] == '_' &&
           name_buf[6] == 'd' &&
           name_buf[7] == 'i' &&
           name_buf[8] == 'r' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [sheet_dir]\n -> string: \"...\"\n"); return; }
    if (arg->_str_len +1 > CORE_PATH_MAX) { _P_ERR("path given to [sheet_dir] is too long\n -> max: %d\n", CORE_PATH_MAX); return; }
    char path[CORE_PATH_MAX];
    STRNCPY(path, arg->_str, arg->_str_len);
    path[arg->_str_len] = '\0';
    int path_len = (int)strlen(path);
    // PF("sheet_path: %s\n", path);
    if (path[path_len -1] != '/' &&
        path[path_len -1] != '\\')
    { strcat(path, "/"); path_len++; }
    // replace '\' with '/'
    for (u32 i = 0; i < strlen(path); ++i)
    { if (path[i] == '\\') { path[i] = '/'; } }
    // PF("sheet_path: %s\n", path);
    if (!check_dir_exists(path)) { _P_ERR("couldnt find path given for [sheet_dir]: %s\n", path); return; }
   
    if (core_data->sheet_paths_len < CORE_SHEET_PATHS_MAX)
    {
      STRCPY(core_data->sheet_paths[core_data->sheet_paths_len], path);
      // P_STR(core_data->sheet_paths[core_data->sheet_paths_len]);
      core_data->sheet_paths_len++;
    }
    else { _P_ERR("cant add additional [sheet_dir] / [sheet_dir_rel] paths, max is %d\n", CORE_SHEET_PATHS_MAX); }
  }
  // [title_color]
  else if (name_buf[ 0] == 't' &&
           name_buf[ 1] == 'i' &&
           name_buf[ 2] == 't' &&
           name_buf[ 3] == 'l' &&
           name_buf[ 4] == 'e' &&
           name_buf[ 5] == '_' &&
           name_buf[ 6] == 'c' &&
           name_buf[ 7] == 'o' &&
           name_buf[ 8] == 'l' &&
           name_buf[ 9] == 'o' &&
           name_buf[10] == 'r' )
  {
    if (arg->type != ARGUMENT_TYPE_COLOR)
    { 
      _P_ERR("incorrect argument format for [title_color]\n"
             "-> black, red, green, yellow, blue, purple, cyan, white\n"
             "-> BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n"); 
      return; 
    }
    core_data->title_color = arg->color;
  }
  // [seperator_left]
  else if (name_buf[ 0] == 's' &&
           name_buf[ 1] == 'e' &&
           name_buf[ 2] == 'p' &&
           name_buf[ 3] == 'e' &&
           name_buf[ 4] == 'r' &&
           name_buf[ 5] == 'a' &&
           name_buf[ 6] == 't' &&
           name_buf[ 7] == 'o' &&
           name_buf[ 8] == 'r' &&
           name_buf[ 9] == '_' &&
           name_buf[10] == 'l' &&
           name_buf[11] == 'e' &&
           name_buf[12] == 'f' &&
           name_buf[13] == 't' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [seperator_left]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [seperator_left] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->seperator_left, arg->_str, arg->_str_len); }
  }
  // [seperator_right]
  else if (name_buf[ 0] == 's' &&
           name_buf[ 1] == 'e' &&
           name_buf[ 2] == 'p' &&
           name_buf[ 3] == 'e' &&
           name_buf[ 4] == 'r' &&
           name_buf[ 5] == 'a' &&
           name_buf[ 6] == 't' &&
           name_buf[ 7] == 'o' &&
           name_buf[ 8] == 'r' &&
           name_buf[ 9] == '_' &&
           name_buf[10] == 'r' &&
           name_buf[11] == 'i' &&
           name_buf[12] == 'g' &&
           name_buf[13] == 'h' &&
           name_buf[14] == 't' )
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [seperator_right]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [seperator_right] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->seperator_right, arg->_str, arg->_str_len); }
  }
  // [border]
  else if (strcmp(name_buf, "border") == 0)
  {
    if (arg->type != ARGUMENT_TYPE_STR)
    { _P_ERR("incorrect argument format for [border]\n -> string: \"...\"\n"); return; }
    if ( arg->_str_len > CORE_ICONS_MAX)
    { 
      _P_ERR("string given to [border] is too long\n -> len: '%d', max: '%d'\n", 
             arg->_str_len, CORE_ICONS_MAX); return; 
    }
    else
    { STRNCPY(core_data->border, arg->_str, arg->_str_len); }
  }
  // [spacing]
  else if (strcmp(name_buf, "title_spacing") == 0)
  {
    if (arg->type != ARGUMENT_TYPE_NUMBER)
    { _P_ERR("incorrect argument format for [title_spacing]\n -> number: 0.5, 1.0, ...\n"); return; }
    // else if ( arg->number > 1.0f || arg->number < 0.0f )
    // { 
    //   _P_ERR("number given to [title_spacing] isnt between 0.0 and 1.0\n -> %f\n", 
    //          arg->number); 
    //   return; 
    // }
    else if ( arg->number < 0.0f )
    { 
      _P_ERR("number given to [title_spacing] isnt less than 0.0 \n -> %f\n", 
             arg->number); 
      return; 
    }
    else
    { core_data->title_spacing = arg->number; }
  }
  else { _P_ERR("unknown argument [%s]\n", name_buf); }

}

config_error_flag config_parse_argument(argument_t* arg, char* buf, int buf_len)
{
  ASSERT(buf_len > 0);
  // P_INT(buf_len);
  arg->type = ARGUMENT_TYPE_INVALID;
  
  // string
  if (buf[0] == '"')
  {
    int i = 1;
    // arg->_str = &buf[i];
    while ( buf[i] != '"' || 
            (buf[i] == '"' && buf[i-1] == '\\') )
    {
      if (buf[i] == '\\' && 
          (buf[i+1] == '\\' ||
           buf[i+1] == '"'  ) 
         )
      { i++; continue; }
      arg->_str[arg->_str_len++] = buf[i];
      i++;
      if (i >= buf_len || buf[i] == '\0' || buf[i] == '\n') 
      { return CONFIG_ERROR_INVALID_STRING; }
      // arg->_str[arg->_str_len++] = buf[i];
    }
    // arg->_str_len = i -1;
    arg->_str[arg->_str_len] = '\0';
    // P_STR(arg->_str);
    arg->type = ARGUMENT_TYPE_STR;
    return CONFIG_ERROR_NO_ERROR; 
  }
  // number 
  else if ( isdigit(buf[0]) )
  {
    int i = 0;
    while (isdigit(buf[i]) || buf[i] == '.')
    {
      arg->_str[arg->_str_len++] = buf[i];
      i++;
      if (i >= buf_len || buf[i] == '\0' || buf[i] == '\n') 
      { break; }
    }
    arg->_str[arg->_str_len] = '\0';
    arg->number = (f32)atof(arg->_str);
    arg->type = ARGUMENT_TYPE_NUMBER;
    return CONFIG_ERROR_NO_ERROR; 
  }
  // bool 
  else if ((value_buf[0] == 'f' || value_buf[0] == 'F') && 
           (value_buf[1] == 'a' || value_buf[1] == 'A') &&
           (value_buf[2] == 'l' || value_buf[2] == 'L') && 
           (value_buf[3] == 's' || value_buf[3] == 'S') &&
           (value_buf[4] == 'e' || value_buf[4] == 'E') && 
           value_buf[5] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_BOOL;
    arg->_bool = false;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 't' || value_buf[0] == 'T') && 
           (value_buf[1] == 'r' || value_buf[1] == 'R') &&
           (value_buf[2] == 'u' || value_buf[2] == 'U') && 
           (value_buf[3] == 'e' || value_buf[3] == 'E') &&
           value_buf[4] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_BOOL;
    arg->_bool = true;
    return CONFIG_ERROR_NO_ERROR; 
  }
  // colors
  else if ((value_buf[0] == 'b' || value_buf[0] == 'B') && 
           (value_buf[1] == 'l' || value_buf[1] == 'L') &&
           (value_buf[2] == 'a' || value_buf[2] == 'A') && 
           (value_buf[3] == 'c' || value_buf[3] == 'C') &&
           (value_buf[4] == 'k' || value_buf[4] == 'K') &&
           value_buf[5] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_BLACK;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'r' || value_buf[0] == 'R') && 
           (value_buf[1] == 'e' || value_buf[1] == 'E') &&
           (value_buf[2] == 'd' || value_buf[2] == 'D') && 
           value_buf[3] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_RED;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'g' || value_buf[0] == 'G') && 
           (value_buf[1] == 'r' || value_buf[1] == 'R') &&
           (value_buf[2] == 'e' || value_buf[2] == 'E') && 
           (value_buf[3] == 'e' || value_buf[3] == 'E') &&
           (value_buf[4] == 'n' || value_buf[4] == 'N') &&
           value_buf[5] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_GREEN;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'y' || value_buf[0] == 'Y') && 
           (value_buf[1] == 'e' || value_buf[1] == 'E') &&
           (value_buf[2] == 'l' || value_buf[2] == 'L') && 
           (value_buf[3] == 'l' || value_buf[3] == 'L') &&
           (value_buf[4] == 'o' || value_buf[4] == 'O') &&
           (value_buf[5] == 'w' || value_buf[5] == 'W') &&
           value_buf[6] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_YELLOW;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'b' || value_buf[0] == 'B') && 
           (value_buf[1] == 'l' || value_buf[1] == 'L') &&
           (value_buf[2] == 'u' || value_buf[2] == 'U') && 
           (value_buf[3] == 'e' || value_buf[3] == 'E') &&
           value_buf[4] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_BLUE;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'p' || value_buf[0] == 'P') && 
           (value_buf[1] == 'u' || value_buf[1] == 'U') &&
           (value_buf[2] == 'r' || value_buf[2] == 'R') && 
           (value_buf[3] == 'p' || value_buf[3] == 'P') &&
           (value_buf[4] == 'l' || value_buf[4] == 'L') &&
           (value_buf[5] == 'e' || value_buf[5] == 'E') &&
           value_buf[6] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_PURPLE;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'c' || value_buf[0] == 'C') && 
           (value_buf[1] == 'y' || value_buf[1] == 'Y') &&
           (value_buf[2] == 'a' || value_buf[2] == 'A') && 
           (value_buf[3] == 'n' || value_buf[3] == 'N') &&
           value_buf[4] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_CYAN;
    return CONFIG_ERROR_NO_ERROR; 
  }
  else if ((value_buf[0] == 'w' || value_buf[0] == 'W') && 
           (value_buf[1] == 'h' || value_buf[1] == 'H') &&
           (value_buf[2] == 'i' || value_buf[2] == 'I') && 
           (value_buf[3] == 't' || value_buf[3] == 'T') &&
           (value_buf[4] == 'e' || value_buf[4] == 'E') &&
           value_buf[5] == '\0')
  { 
    arg->type  = ARGUMENT_TYPE_COLOR;
    arg->color = PF_WHITE;
    return CONFIG_ERROR_NO_ERROR; 
  }

  return CONFIG_ERROR_INVALID_TYPE;
}

