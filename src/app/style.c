#include "app/style.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void style_highlight_c(char** txt_ptr, char** buf_ptr, u32* buf_pos_ptr, u32* i_ptr)
{
  #define txt     (*txt_ptr)
  #define buf     (*buf_ptr)
  #define buf_pos (*buf_pos_ptr)
  #define i       (*i_ptr)
  
  #define BUF_DUMP()        \
    buf[buf_pos] = '\0';  \
    PF("%s", buf);        \
    buf_pos = 0;
  
  #define DUMP_COLORED(n, col)                              \
    { BUF_DUMP();                                         \
      PF_COLOR(col);                                        \
      int j = 0;                                            \
      while (j < (n)) { buf[buf_pos++] = txt[i++]; j++; }   \
      BUF_DUMP();                                           \
      PF_COLOR(PF_WHITE); }

  // -- types --
  if (!isalnum(txt[i -1]) && txt[i -1] != '|')
  {
    if (txt[i +0] == 'v' && txt[i +1] == 'o' &&
        txt[i +2] == 'i' && txt[i +3] == 'd')
    { DUMP_COLORED(4, COL_TYPE); }
    if (txt[i +0] == 'i' && txt[i +1] == 'n' &&
        txt[i +2] == 't')
    { DUMP_COLORED(3, COL_TYPE); }
    if (txt[i +0] == 'f' && txt[i +1] == 'l' &&
        txt[i +2] == 'o' && txt[i +3] == 'a' &&
        txt[i +4] == 't')
    { DUMP_COLORED(5, COL_TYPE); }
    if (txt[i +0] == 'd' && txt[i +1] == 'o' &&
        txt[i +2] == 'u' && txt[i +3] == 'b' &&
        txt[i +4] == 'l' && txt[i +5] == 'e')
    { DUMP_COLORED(6, COL_TYPE); }
    if (txt[i +0] == 's' && txt[i +1] == 'h' &&
        txt[i +2] == 'o' && txt[i +3] == 'r' &&
        txt[i +4] == 't')
    { DUMP_COLORED(5, COL_TYPE); }
    if (txt[i +0] == 'l' && txt[i +1] == 'o' &&
        txt[i +2] == 'n' && txt[i +3] == 'g')
    { DUMP_COLORED(4, COL_TYPE); }
    if (txt[i +0] == 's' && txt[i +1] == 'i' &&
        txt[i +2] == 'z' && txt[i +3] == 'e' &&
        txt[i +4] == '_' && txt[i +5] == 't')
    { DUMP_COLORED(6, COL_TYPE); }
    if (txt[i +0] == 'u' && txt[i +1] == 'n' &&
        txt[i +2] == 's' && txt[i +3] == 'i' &&
        txt[i +4] == 'g' && txt[i +5] == 'n' &&
        txt[i +6] == 'e' && txt[i +7] == 'd')
    { DUMP_COLORED(8, COL_TYPE); }
    if (txt[i +0] == 'e' && txt[i +1] == 'n' &&
        txt[i +2] == 'u' && txt[i +3] == 'm')
    { DUMP_COLORED(4, COL_TYPE); }
    if (txt[i +0] == 's' && txt[i +1] == 't' &&
        txt[i +2] == 'r' && txt[i +3] == 'u' &&
        txt[i +4] == 'c' && txt[i +5] == 't')
    { DUMP_COLORED(6, COL_TYPE); }
    if (txt[i +0] == 'c' && txt[i +1] == 'h' &&
        txt[i +2] == 'a' && txt[i +3] == 'r')
    { DUMP_COLORED(4, COL_TYPE); }
    if (txt[i +0] == 'b' && txt[i +1] == 'o' &&
        txt[i +2] == 'o' && txt[i +3] == 'l')
    { DUMP_COLORED(4, COL_TYPE); }
    if (txt[i +0] == 'c' && txt[i +1] == 'o' &&
        txt[i +2] == 'n' && txt[i +3] == 's' &&
        txt[i +4] == 't')
    { DUMP_COLORED(5, COL_TYPE); }
  }
  // -- values --

  if (isdigit(txt[i]))                      // numbers
  {
    BUF_DUMP();
    PF_COLOR(COL_VALUE);
    int j = 0;
    while (isdigit(txt[i +j]) || txt[i +j] == '.' || txt[i +j] == 'f') { j++; }
    while (j > 0) { buf[buf_pos++] = txt[i++]; j--; }
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
  if (txt[i +0] == 't' && txt[i +1] == 'r' &&
      txt[i +2] == 'u' && txt[i +3] == 'e')
  { DUMP_COLORED(4, COL_VALUE); }
  if (txt[i +0] == 'f' && txt[i +1] == 'a' &&
      txt[i +2] == 'l' && txt[i +3] == 's' &&
      txt[i +4] == 'e')
  { DUMP_COLORED(5, COL_VALUE); }
  if (txt[i +0] == 'N' && txt[i +1] == 'U' &&
      txt[i +2] == 'L' && txt[i +3] == 'L')
  { DUMP_COLORED(4, COL_VALUE); }
  if (txt[i] == '"')                        // strings
  {
    BUF_DUMP();
    PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[i++];
    while (txt[i] != '"') { buf[buf_pos++] = txt[i++]; }
    buf[buf_pos++] = txt[i++];
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
  if (txt[i] == '\'' && txt[i +2] == '\'')  // chars
  {
    BUF_DUMP();
    PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[i++];
    buf[buf_pos++] = txt[i++];
    buf[buf_pos++] = txt[i++];
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }

}
