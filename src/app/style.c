#include "app/style.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KEY_MAX 32
const u32  key_types_len = 12;   // @TODO: do this automatically
const char key_types[][KEY_MAX] = { "unsigned", "const", "void", "char", "short", "int", "long", "float", "double", "struct", "enum", "typedef" };
const u32  key_flow_ctrl_len = 7;
const char key_flow_ctrl[][KEY_MAX] = { "if", "else", "for", "while", "switch", "break", "continue" };
const u32  key_values_len = 3;
const char key_values[][KEY_MAX] = { "NULL", "true", "false" };


// check if char is valid as an ending for a type name
#define TYPE_START(c)    (isspace(c) || (c) == ',' || (c) == ';' || (c) == '(')
// check if char is valid as an ending for a type name
#define TYPE_END(c)      (isspace(c) || (c) == '*' || !isalnum(c))

// eisther if[(] or if[ ] or else[ if] or [else]
#define FLOW_CTRL_END(c) (txt[c] == '(' ||                                \
                         (isspace(txt[c]) && txt[(c) +1] == '(')  ||      \
                         (isspace(txt[c]) && txt[(c) +1] == 'i'   &&      \
                          txt[(c) +2] == 'f' && isspace(txt[c]))  ||      \
                         (txt[(c) -4] == 'e' && txt[(c) -3] == 'l' &&     \
                         txt[(c) -2] == 's' && txt[(c) -1] == 'e'))

// check if char is valid as an ending for a value name
#define VALUE_END(c)      (!isalnum(c))


void style_highlight_c(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr) 
{
  #define buf_pos (*buf_pos_ptr)
  #define i       (*i_ptr)
  
  #define BUF_DUMP()        \
      buf[buf_pos] = '\0';  \
      PF("%s", buf);        \
      buf_pos = 0;
 
  #define BUF_DUMP_OFFSET(offset) \
      buf[buf_pos] = '\0';        \
      PF("%s", buf +(offset));    \
      buf_pos = 0;
  
  #define DUMP_COLORED(n, col)                              \
      { BUF_DUMP();                                         \
      PF_COLOR(col);                                        \
      int j = 0;                                            \
      while (j < (n)) { buf[buf_pos++] = txt[i++]; j++; }   \
      BUF_DUMP();                                           \
      PF_COLOR(PF_WHITE); }

    // PF("%c", txt[i]);

  if (TYPE_START(txt[i -1])) //  && txt[i -1] != '|') //  && !in_tag)  
  {
    // -- types --
    for (u32 s = 0; s < key_types_len; ++s) // string
    {
      // @UNSURE: could use strcmp()
      bool equal = false;
      u32 len = strlen(key_types[s]);
      u32 c;
      for (c = 0; c < len; ++c)             // char
      {
        equal = txt[i +c] == key_types[s][c];
        if (!equal) { break; }
      }
      if (equal && TYPE_END(txt[i +len]))
      {
        DUMP_COLORED(len, COL_TYPE); 
        return;;
      }
    }

    // -- flow control --
    for (u32 s = 0; s < key_flow_ctrl_len; ++s) // string
    {
      // @UNSURE: could use strcmp()
      bool equal = false;
      u32 len = strlen(key_flow_ctrl[s]);
      u32 c;
      for (c = 0; c < len; ++c)             // char
      {
        equal = txt[i +c] == key_flow_ctrl[s][c];
        if (!equal) { break; }
      }
      if (equal && FLOW_CTRL_END(i +len))
      {
        DUMP_COLORED(len, COL_TYPE);
        break;
      }
    }
    
    // -- values --
    for (u32 s = 0; s < key_values_len; ++s) // string
    {
      // @UNSURE: could use strcmp()
      bool equal = false;
      u32 len = strlen(key_values[s]);
      u32 c;
      for (c = 0; c < len; ++c)             // char
      {
        equal = txt[i +c] == key_values[s][c];
        if (!equal) { break; }
      }
      if (equal && VALUE_END(txt[i +len]))
      {
        DUMP_COLORED(len, COL_VALUE); 
        return;;
      }
    }

  }
  
  // -- functions --
  int j = 0;
  while(isalnum(txt[i +j]) || txt[i +j] == '_') 
  { j++; }
  if (txt[i +j] == '(' || (txt[i +j] == ' ' && txt[i +j +1] == '('))
  {
    BUF_DUMP();
    PF_COLOR(COL_FUNC);
    while (j > 0) { buf[buf_pos++] = txt[i++]; j--; }
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
 
  // -- values --
  if (isdigit(txt[i]) && !isalpha(txt[i -1]))  // && 
      // (!isalpha(txt[i +1]) || txt[i +1] == 'f')) //  && 
      // !in_tag)                                      // numbers
  {
    BUF_DUMP();
    PF_COLOR(COL_VALUE);
    int j = 0;
    while (isdigit(txt[i +j]) || txt[i +j] == '.' || txt[i +j] == 'f' || 
           (j == 1 && txt[i] == '0' && (txt[i +j] == 'b' || txt[i +j] == 'x'))) // 0x687, 0b1010
    { j++; }
    while (j > 0) { buf[buf_pos++] = txt[i++]; j--; }
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
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

  // -- comments --
  if (txt[i] == '/' && txt[i +1] == '/')
  {
    BUF_DUMP();
    while (txt[i] != '\n') { buf[buf_pos++] = txt[i++]; }
    PF_STYLE(PF_ITALIC);
    PF_STYLE_COL(PF_DIM, COL_COMMENT);
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
  if (txt[i] == '/' && txt[i +1] == '*')
  {
    BUF_DUMP();
    while (txt[i -2] != '*' || txt[i -1] != '/') { buf[buf_pos++] = txt[i++]; }
    PF_STYLE(PF_ITALIC);
    PF_STYLE_COL(PF_DIM, COL_COMMENT);
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  } 

  // -- macros --
  if (txt[i] == '#')
  {
    PF_COLOR(COL_TYPE);
    BUF_DUMP();
    while (txt[i] != '\n' && txt[i] != '\0') { buf[buf_pos++] = txt[i++]; }
    PF_COLOR(COL_MACRO);
    BUF_DUMP();
    PF_COLOR(PF_WHITE);
  }
}


/*

// void style_highlight_c(char** txt_ptr, char** buf_ptr, u32* buf_pos_ptr, u32* i_ptr)
// {
//   #define txt     (*txt_ptr)
//   #define buf     (*buf_ptr)
//   #define buf_pos (*buf_pos_ptr)
//   #define i       (*i_ptr)
//   
//   #define BUF_DUMP()        \
//     buf[buf_pos] = '\0';  \
//     PF("%s", buf);        \
//     buf_pos = 0;
//   
//   #define DUMP_COLORED(n, col)                              \
//     { BUF_DUMP();                                         \
//       PF_COLOR(col);                                        \
//       int j = 0;                                            \
//       while (j < (n)) { buf[buf_pos++] = txt[i++]; j++; }   \
//       BUF_DUMP();                                           \
//       PF_COLOR(PF_WHITE); }
// 
//   // -- types --
//   if (!isalnum(txt[i -1]) && txt[i -1] != '|')
//   {
//     if (txt[i +0] == 'v' && txt[i +1] == 'o' &&
//         txt[i +2] == 'i' && txt[i +3] == 'd')
//     { DUMP_COLORED(4, COL_TYPE); }
//     if (txt[i +0] == 'i' && txt[i +1] == 'n' &&
//         txt[i +2] == 't')
//     { DUMP_COLORED(3, COL_TYPE); }
//     if (txt[i +0] == 'f' && txt[i +1] == 'l' &&
//         txt[i +2] == 'o' && txt[i +3] == 'a' &&
//         txt[i +4] == 't')
//     { DUMP_COLORED(5, COL_TYPE); }
//     if (txt[i +0] == 'd' && txt[i +1] == 'o' &&
//         txt[i +2] == 'u' && txt[i +3] == 'b' &&
//         txt[i +4] == 'l' && txt[i +5] == 'e')
//     { DUMP_COLORED(6, COL_TYPE); }
//     if (txt[i +0] == 's' && txt[i +1] == 'h' &&
//         txt[i +2] == 'o' && txt[i +3] == 'r' &&
//         txt[i +4] == 't')
//     { DUMP_COLORED(5, COL_TYPE); }
//     if (txt[i +0] == 'l' && txt[i +1] == 'o' &&
//         txt[i +2] == 'n' && txt[i +3] == 'g')
//     { DUMP_COLORED(4, COL_TYPE); }
//     if (txt[i +0] == 's' && txt[i +1] == 'i' &&
//         txt[i +2] == 'z' && txt[i +3] == 'e' &&
//         txt[i +4] == '_' && txt[i +5] == 't')
//     { DUMP_COLORED(6, COL_TYPE); }
//     if (txt[i +0] == 'u' && txt[i +1] == 'n' &&
//         txt[i +2] == 's' && txt[i +3] == 'i' &&
//         txt[i +4] == 'g' && txt[i +5] == 'n' &&
//         txt[i +6] == 'e' && txt[i +7] == 'd')
//     { DUMP_COLORED(8, COL_TYPE); }
//     if (txt[i +0] == 'e' && txt[i +1] == 'n' &&
//         txt[i +2] == 'u' && txt[i +3] == 'm')
//     { DUMP_COLORED(4, COL_TYPE); }
//     if (txt[i +0] == 's' && txt[i +1] == 't' &&
//         txt[i +2] == 'r' && txt[i +3] == 'u' &&
//         txt[i +4] == 'c' && txt[i +5] == 't')
//     { DUMP_COLORED(6, COL_TYPE); }
//     if (txt[i +0] == 'c' && txt[i +1] == 'h' &&
//         txt[i +2] == 'a' && txt[i +3] == 'r')
//     { DUMP_COLORED(4, COL_TYPE); }
//     if (txt[i +0] == 'b' && txt[i +1] == 'o' &&
//         txt[i +2] == 'o' && txt[i +3] == 'l')
//     { DUMP_COLORED(4, COL_TYPE); }
//     if (txt[i +0] == 'c' && txt[i +1] == 'o' &&
//         txt[i +2] == 'n' && txt[i +3] == 's' &&
//         txt[i +4] == 't')
//     { DUMP_COLORED(5, COL_TYPE); }
//   }
//   // -- values --
// 
//   if (isdigit(txt[i]))                      // numbers
//   {
//     BUF_DUMP();
//     PF_COLOR(COL_VALUE);
//     int j = 0;
//     while (isdigit(txt[i +j]) || txt[i +j] == '.' || txt[i +j] == 'f') { j++; }
//     while (j > 0) { buf[buf_pos++] = txt[i++]; j--; }
//     BUF_DUMP();
//     PF_COLOR(PF_WHITE);
//   }
//   if (txt[i +0] == 't' && txt[i +1] == 'r' &&
//       txt[i +2] == 'u' && txt[i +3] == 'e')
//   { DUMP_COLORED(4, COL_VALUE); }
//   if (txt[i +0] == 'f' && txt[i +1] == 'a' &&
//       txt[i +2] == 'l' && txt[i +3] == 's' &&
//       txt[i +4] == 'e')
//   { DUMP_COLORED(5, COL_VALUE); }
//   if (txt[i +0] == 'N' && txt[i +1] == 'U' &&
//       txt[i +2] == 'L' && txt[i +3] == 'L')
//   { DUMP_COLORED(4, COL_VALUE); }
//   if (txt[i] == '"')                        // strings
//   {
//     BUF_DUMP();
//     PF_COLOR(COL_VALUE);
//     buf[buf_pos++] = txt[i++];
//     while (txt[i] != '"') { buf[buf_pos++] = txt[i++]; }
//     buf[buf_pos++] = txt[i++];
//     BUF_DUMP();
//     PF_COLOR(PF_WHITE);
//   }
//   if (txt[i] == '\'' && txt[i +2] == '\'')  // chars
//   {
//     BUF_DUMP();
//     PF_COLOR(COL_VALUE);
//     buf[buf_pos++] = txt[i++];
//     buf[buf_pos++] = txt[i++];
//     buf[buf_pos++] = txt[i++];
//     BUF_DUMP();
//     PF_COLOR(PF_WHITE);
//   }
// 
// }

*/

