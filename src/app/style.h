#ifndef STYLE_H
#define STYLE_H

#include "app/core_data.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define BORDER_CHAR '|'

#define MAX_LINE_WIDTH 90

#define COL_TAG               PF_CYAN 
#define STYLE_TAG             PF_UNDERLINE
#define COL_WARNING           PF_RED  
#define STYLE_WARNING         PF_ITALIC 
#define COL_INFO              PF_PURPLE 
#define STYLE_INFO            PF_ITALIC 
#define COL_LINK              PF_BLUE 
#define STYLE_LINK            PF_ITALIC 
#define COL_EXAMPLE           PF_CYAN 


#define COL_COMMENT           PF_WHITE
#define COL_COMMENT_HIGHLIGHT PF_PURPLE
#define COL_FUNC              PF_BLUE 
#define COL_TYPE              PF_GREEN 
#define COL_VALUE             PF_YELLOW
#define COL_MACRO             PF_PURPLE
#define COL_ATTRIBUTE         PF_PURPLE


#define DOC_PF(...)                   fprintf(core_data->pf_out, __VA_ARGS__)

#define _DOC_PF_MODE(style, fg, bg)   DOC_PF("\033[%d;%d;%dm", style, fg, bg)
// @DOC: setting terminal output to a specific mode and text color
#define _DOC_PF_STYLE(style, color)   DOC_PF("\033[%d;%dm", style, color)
// @DOC: setting terminal output to a specific text color
#define _DOC_PF_COLOR(color)          _DOC_PF_STYLE(PF_NORMAL, color)
// @DOC: setting terminal output to default mode, text and background color
#define _DOC_PF_STYLE_RESET()         _DOC_PF_STYLE(PF_NORMAL, PF_WHITE)
#define _DOC_PF_MODE_RESET()          _DOC_PF_MODE(PF_NORMAL, PF_WHITE, PF_BG_BLACK)
#define _DOC_PF_RESET()               _DOC_PF_MODE_RESET();
// @DOC: setting terminal output to default mode and text
#define DOC_PF_COLOR(c)           if (core_data->style_act) { _DOC_PF_COLOR((c)); }
#define DOC_PF_STYLE(s, c)        if (core_data->style_act) { _DOC_PF_STYLE((s), (c)); }
#define DOC_PF_MODE(s, _fg, _bg)  if (core_data->style_act) { _DOC_PF_MODE((s), (_fg), (_bg)); }
#define DOC_PF_RESET()            if (core_data->style_act) { _DOC_PF_MODE_RESET();  } 
#define DOC_PF_MODE_RESET()       DOC_PF_RESET() 
#define DOC_PF_STYLE_RESET()      DOC_PF_RESET() 

#define DOC_P_INT(var)                        \
  if (core_data->style_act) { P_INT(var); }   \
  else { PF("%s: %d\n", #var, (var)); }


INLINE void style_draw_title(char* title)
{
  // core_data_t* core_data = core_data_get();
  int w, h; io_util_get_console_size_win(&w, &h);

 
  int fg = core_data->title_color;
  int bg = fg +10;
  if (core_data->use_utf8 && core_data->use_icons)
  { 
    int width = 0;
    width += strlen(title) +2;
    int space_l;
    int space_r;
    if (core_data->title_spacing <= 1.0f)
    {
      space_l = (f32)(w - width) * core_data->title_spacing; 
      space_r = (f32)(w - width) * (1.0f - core_data->title_spacing); 
    }
    else
    {
      space_l = core_data->title_spacing;
      space_r = (w - width) - core_data->title_spacing;
    }
    DOC_PF("\n");
    DOC_PF_MODE(PF_NORMAL, fg, PF_BG_BLACK);
    for (int i = 0; i < space_l; ++i) { DOC_PF("%s", core_data->border); }
    // DOC_PF(""); 
    DOC_PF("%s", core_data->seperator_left); 
    DOC_PF_MODE(PF_NORMAL, PF_BLACK, bg);
    DOC_PF("%s", title); 
    DOC_PF_MODE(PF_NORMAL, fg, PF_BG_BLACK);
    // DOC_PF(""); 
    DOC_PF("%s", core_data->seperator_right); 
    for (int i = 0; i < space_r; ++i) { DOC_PF("%s", core_data->border); }
    DOC_PF_MODE_RESET();
    DOC_PF("\n");
  }
  else // no icons
  {
    DOC_PF_COLOR(fg);
    P_LINE_STR("%s ", title); 
    DOC_PF_COLOR(PF_WHITE);
  }
}
INLINE void style_draw_line()
{
  // core_data_t* core_data = core_data_get();
  int w, h; io_util_get_console_size_win(&w, &h);

  int fg = core_data->title_color;
  // int bg = fg +10;

  if (core_data->use_utf8 && core_data->use_icons)
  {
    DOC_PF_MODE(PF_NORMAL, fg, PF_BG_BLACK);
    for (int i = 0; i < w; ++i) { DOC_PF("%s", core_data->border); }
    DOC_PF_MODE_RESET();
    DOC_PF("\n");
  }
  else // no icons
  {
    DOC_PF_COLOR(fg);
    P_LINE(); 
    DOC_PF_COLOR(PF_WHITE);
  }
}

// pass the txt to be modified as well as the buffer the sections from txt
// are written to, also pointers to the buffer position and i (iterator)
// of the for loop in which this should be called
// e.g.
//    char* txt = load_file()
//    char  buf[512];
//    int   buf_pos = 0;
//    for (int i = 0; i < len -1; ++i)
//    {
//      // ...
//      style_highlight_c(txt, buf, &buf_pos, &i)
//     }
// return is whether to conzinue in that loop
bool style_highlight_c(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr); 

void style_highlight_c_comment(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr);

void style_print_text_box(const char* box_indent, int box_width, int box_height, bool print_line_nr, const char* file, const int file_len, const char* name, int name_len);

#ifdef __cplusplus
}   // extern c
#endif

#endif
