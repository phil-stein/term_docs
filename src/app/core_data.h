#ifndef CORE_DATA_H
#define CORE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define CORE_PATH_MAX   512
#define CORE_SHEET_PATHS_MAX 8

#define CORE_ICONS_MAX 8

typedef enum
{
  LANG_C,
  LANG_ODIN,
  LANG_TYPE_MAX,

} LANG_TYPE;

typedef struct core_data_t
{
  char exec_path[CORE_PATH_MAX];      // path to executable
  char config_path[CORE_PATH_MAX];    // path to config file
  // set in config.doc now
  // char sheets_path[CORE_PATH_MAX];    // path to sheets folder

  // where DOC_PF(), DOC_PF_COLOR(), etc. print to usually stdout
  // gets initially set in main.c, afterwards used to call doc from 
  // within doc for $doc:"malloc"$
  FILE* pf_out;   

  bool style_act;                     // use syntax highlighting
  bool style_deact_cmd;               // -c/-color command
  bool print_loc_act;                 // -loc command
  bool use_utf8;
  bool use_icons;
  LANG_TYPE lang_type;

  char error_icon[CORE_ICONS_MAX];
  char warning_icon[CORE_ICONS_MAX];
  char info_icon[CORE_ICONS_MAX];
  char link_icon[CORE_ICONS_MAX];

  int  title_color;
  f32  title_spacing;
  char seperator_left[CORE_ICONS_MAX];
  char seperator_right[CORE_ICONS_MAX];
  char border[CORE_ICONS_MAX];

  char sheet_paths[CORE_SHEET_PATHS_MAX][CORE_PATH_MAX];
  int  sheet_paths_len;

  char text_box_indent[CORE_ICONS_MAX];
  int  text_box_width;
  int  text_box_height;
}core_data_t;
#define CORE_DATA_INIT()                  \
{                                         \
  .pf_out /*getssetinmain*/ =NULL,        \
  .style_act                = true,       \
  .style_deact_cmd          = false,      \
  .print_loc_act            = false,      \
  .use_utf8                 = false,      \
  .use_icons                = false,      \
  .lang_type                = LANG_C,     \
  .error_icon               = "!",        \
  .warning_icon             = "!",        \
  .info_icon                = "~",        \
  .link_icon                = "?",        \
  .title_color              = PF_PURPLE,  \
  .title_spacing            = 0.5f,       \
  .seperator_left           = "",        \
  .seperator_right          = "",        \
  .border                   = "",        \
  .sheet_paths_len          = 0,          \
  .text_box_indent          = "    ",     \
  .text_box_width           = 80,         \
  .text_box_height          = 40,         \
}
  // .border                 = "",    \x

extern core_data_t* core_data;
// core_data_t* core_data_get();

void core_data_init_config_path();

void core_data_init_sheet_path(); // not used

#ifdef __cplusplus
}   // extern c
#endif

#endif
