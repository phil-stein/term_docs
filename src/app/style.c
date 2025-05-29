#include "app/style.h"
#include "app/doc.h"
#include "app/core_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KEY_MAX 32

const char key_types[][KEY_MAX] = 
{ 
  "unsigned", "const", "extern", "restrict", "volatile", "static", "auto", "register", "signed", 
  "void", "char", "short", "int", "long", "float", "double", "bool", 
  "struct", "enum", "typedef",
  "size_t", "rsize_t", "errno_t", "FILE",
  "u8", "u16", "u32", "u64", "u8", "s16", "s32", "s64", "f32", "f64"
};
const u32  key_types_len = sizeof(key_types) / sizeof(key_types[0]);   

const char key_flow_ctrl[][KEY_MAX] = { "if", "else", "for", "while", "do", "switch"};
const u32  key_flow_ctrl_len = sizeof(key_flow_ctrl) / sizeof(key_flow_ctrl[0]);
// @TODO: break & conmtinue dont get highlighted because they dont have () at the end
const char key_flow_ctrl_cmd[][KEY_MAX] = { "return", "break", "continue", "case", "default", "goto" };
const u32  key_flow_ctrl_cmd_len = sizeof(key_flow_ctrl_cmd) / sizeof(key_flow_ctrl_cmd[0]);

const char key_values[][KEY_MAX] = { "NULL", "true", "false" };
const u32  key_values_len = sizeof(key_values) / sizeof(key_values[0]);

const char key_comment[][KEY_MAX] = { "@DOC:", "@TODO:", "@NOTE:", "@TMP:", "@BUGG:", "@OPTIMIZATION:" };
const u32  key_comment_len = sizeof(key_comment) / sizeof(key_comment[0]);

// check if char is valid as an ending for a type name
#define TYPE_START(c)    (isspace(c) || (c) == ',' || (c) == ';' || (c) == '(')
// check if char is valid as an ending for a type name
#define TYPE_END(c)      (isspace(c) || (c) == '*' || !isalnum(c))

// give char idx into txt c and bool ptr as return 
#define FLOW_CTRL_CMD_END_RETURN(c, b) { u32 _c = c; while(txt[_c] != '\n' && txt[_c] != '\0') \
                                       { if (txt[_c] == ';') {(*b) = true; break;} _c++; } }
#define FLOW_CTRL_CMD_END_CASE(c, b)   { u32 _c = c; while(txt[_c] != '\n' && txt[_c] != '\0') \
                                       { if (txt[_c] == ':') {(*b) = true; break;} _c++; } }
#define FLOW_CTRL_CMD_END(c)  ((c) == ';')

// eisther if[(] or if[ ] or else[ if] or [else]
#define FLOW_CTRL_END(c) (txt[c] == '(' ||                                \
                         (isspace(txt[c]) && txt[(c) +1] == '(')  ||      \
                         (isspace(txt[c]) && txt[(c) +1] == 'i'   &&      \
                          txt[(c) +2] == 'f' && isspace(txt[c])) )

// check if char is valid as an ending for a value name
#define VALUE_END(c)      (!isalnum(c))


#define BUF_DUMP()        \
    buf[buf_pos] = '\0';  \
    DOC_PF("%s", buf);        \
    buf_pos = 0;

#define BUF_DUMP_OFFSET(offset) \
    buf[buf_pos] = '\0';        \
    DOC_PF("%s", buf +(offset));    \
    buf_pos = 0;

#define DUMP_COLORED(n, col)                                      \
    if (core_data->style_act)                                     \
    {                                                             \
      BUF_DUMP();                                                 \
      DOC_PF_COLOR(col);                                          \
      int j = 0;                                                  \
      while (j < (n)) { buf[buf_pos++] = txt[txt_pos++]; j++; }   \
      BUF_DUMP();                                                 \
      DOC_PF_COLOR(PF_WHITE);                                     \
      DOC_PF_STYLE_RESET();                                       \
    } else                                                        \
    {                                                             \
      BUF_DUMP();                                                 \
      int j = 0;                                                  \
      while (j < (n)) { buf[buf_pos++] = txt[txt_pos++]; j++; }   \
      BUF_DUMP();                                                 \
    }



#define buf_pos (*buf_pos_ptr)
#define txt_pos       (*i_ptr)
bool style_highlight_c(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr) 
{
  // core_data_t* core_data = core_data_get();

  // buf_pos & i are macros for _ptr equivalent

  // -- types --
  if (txt_pos == 0 || TYPE_START(txt[txt_pos -1])) //  && txt[i -1] != '|') //  && !in_tag)  
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
        equal = txt[txt_pos +c] == key_types[s][c];
        if (!equal) { break; }
      }
      if (equal && TYPE_END(txt[txt_pos +len]))
      {
        DUMP_COLORED(len, COL_TYPE); 
        return false;
      }
    }

    // -- flow control --
    for (u32 s = 0; s < key_flow_ctrl_cmd_len; ++s) // string
    {
      // @UNSURE: could use strcmp()
      bool equal = false;
      u32 len = strlen(key_flow_ctrl_cmd[s]);
      u32 c;
      for (c = 0; c < len; ++c)             // char
      {
        equal = txt[txt_pos +c] == key_flow_ctrl_cmd[s][c];
        if (!equal) { break; }
      }
      bool return_end = false;
      FLOW_CTRL_CMD_END_RETURN(txt_pos+len, &return_end);
      bool case_end = false;
      FLOW_CTRL_CMD_END_CASE(txt_pos+len, &case_end);
      if (equal && (( return_end || case_end) || 
                      FLOW_CTRL_CMD_END(txt_pos +len)))
      {
        DUMP_COLORED(len, COL_TYPE);
        return false;
      }
    }
    for (u32 s = 0; s < key_flow_ctrl_len; ++s) // string
    {
      // @UNSURE: could use strcmp()
      bool equal = false;
      u32 len = strlen(key_flow_ctrl[s]);
      u32 c;
      for (c = 0; c < len; ++c)             // char
      {
        equal = txt[txt_pos +c] == key_flow_ctrl[s][c];
        if (!equal) { break; }
      }
      bool else_end = false;
      u32 _c = txt_pos +len;
      else_end = txt[txt_pos +len] == '{';
      while(isspace(txt[_c]) && txt[_c] != '\0')
      { if (txt[_c +1] == '{') { else_end = true; break;} _c++; }
      
    if (equal && (else_end || FLOW_CTRL_END(txt_pos +len)))
      {
        DUMP_COLORED(len, COL_TYPE);
        return false;
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
        equal = txt[txt_pos +c] == key_values[s][c];
        if (!equal) { break; }
      }
      if (equal && VALUE_END(txt[txt_pos +len]))
      {
        DUMP_COLORED(len, COL_VALUE); 
        return false;
      }
    }

  }
  
  // -- functions --
  int j = 0;
  while(isalnum(txt[txt_pos +j]) || txt[txt_pos +j] == '_') 
  { j++; }
  if (txt[txt_pos +j] == '(' || (txt[txt_pos +j] == ' ' && txt[txt_pos +j +1] == '('))
  {
    BUF_DUMP();
    DOC_PF_COLOR(COL_FUNC);
    while (j > 0) { buf[buf_pos++] = txt[txt_pos++]; j--; }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
 
  // -- values --
  // if (isdigit(txt[i]) && !isalpha(txt[i -1]))  
  if (isdigit(txt[txt_pos]) && (isspace(txt[txt_pos -1]) || txt[txt_pos -1] == '+' || txt[txt_pos -1] == '-') && !isalpha(txt[txt_pos -1]))  
  {
    BUF_DUMP();
    DOC_PF_COLOR(COL_VALUE);
    int j = 0;
    while (isdigit(txt[txt_pos +j]) || txt[txt_pos +j] == '.' || txt[txt_pos +j] == 'f' || 
           (j == 1 && txt[txt_pos] == '0' && (txt[txt_pos +j] == 'b' || txt[txt_pos +j] == 'x'))) // 0x687, 0b1010
    { j++; }
    while (j > 0) { buf[buf_pos++] = txt[txt_pos++]; j--; }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  else if (txt[txt_pos] == '"')                        // strings
  {
    BUF_DUMP();
    DOC_PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[txt_pos++];
    while (txt[txt_pos] != '"') { buf[buf_pos++] = txt[txt_pos++]; }  // || txt[i+1] == '"'
    buf[buf_pos++] = txt[txt_pos++]; // i++
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
    txt_pos--;
    return true;
  }
  else if (txt[txt_pos] == '\'' && txt[txt_pos +2] == '\'')  // chars
  {
    BUF_DUMP(); DOC_PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    buf[buf_pos++] = txt[txt_pos++];
    BUF_DUMP(); DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  else if (txt[txt_pos] == '\'' && txt[txt_pos+1] == '\\' && txt[txt_pos +3] == '\'')  // escaped chars
  {
    BUF_DUMP(); DOC_PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    BUF_DUMP(); DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  else if (txt[txt_pos] == '\'' && txt[txt_pos+1] == 'u' && txt[txt_pos +6] == '\'')  // unicode chars
  {
    BUF_DUMP(); DOC_PF_COLOR(COL_VALUE);
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    buf[buf_pos++] = txt[txt_pos++]; buf[buf_pos++] = txt[txt_pos++];
    buf[buf_pos++] = txt[txt_pos++];
    BUF_DUMP(); DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }

  // -- comments --
  else if (txt[txt_pos] == '/' && txt[txt_pos +1] == '/')
  {
    BUF_DUMP();
    DOC_PF_STYLE(PF_ITALIC, COL_COMMENT);
    DOC_PF_STYLE(PF_DIM, COL_COMMENT);
    while (txt[txt_pos] != '\n') 
    { 
      if ((txt[txt_pos] == '\\' && txt[txt_pos+1] == '\n') ||
          (txt[txt_pos] == '\\' && txt[txt_pos+1] == '\r') ) // multiline, macros, comments, has to be done here
      {
        buf[buf_pos++] = txt[txt_pos++];  // add '\'
        buf[buf_pos++] = txt[txt_pos++];  // add '\n'
        BUF_DUMP();
      }

      style_highlight_c_comment(txt, buf, &buf_pos, &txt_pos);
      // buf[buf_pos++] = txt[i++]; 
      bool skip_char_tmp = false;
      doc_color_code_escape_chars(txt, buf, &buf_pos, &txt_pos, &skip_char_tmp);
      if (!skip_char_tmp) { buf[buf_pos++] = txt[txt_pos++]; }
      else { txt_pos++; }
    }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  else if (txt[txt_pos] == '/' && txt[txt_pos +1] == '*')
  {
    BUF_DUMP();
    DOC_PF_STYLE(PF_ITALIC, COL_COMMENT);
    DOC_PF_STYLE(PF_DIM, COL_COMMENT);
    while (txt[txt_pos -2] != '*' || txt[txt_pos -1] != '/') 
    {
      style_highlight_c_comment(txt, buf, &buf_pos, &txt_pos);
      // buf[buf_pos++] = txt[i++]; 
      bool skip_char_tmp = false;
      doc_color_code_escape_chars(txt, buf, &buf_pos, &txt_pos, &skip_char_tmp);
      if (!skip_char_tmp) { buf[buf_pos++] = txt[txt_pos++]; }
      else { txt_pos++; }
    }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  } 

  // -- macros --
  else if (txt[txt_pos] == '#')  
  {
    // DOC_PF_COLOR(COL_TYPE);
    BUF_DUMP();
    DOC_PF_COLOR(COL_MACRO);
    while (txt[txt_pos] != '\n' && txt[txt_pos] != '\0')
    { 
      // multiline, macros, comments, has to be done here
      if ((txt[txt_pos] == '\\' && txt[txt_pos+1] == '\n') ||   // LF,    unix
          (txt[txt_pos] == '\\' && txt[txt_pos+1] == '\r') )    // CRLF   win
      {
        buf[buf_pos++] = txt[txt_pos++];  // add '\'
        buf[buf_pos++] = txt[txt_pos++];  // add '\n'
        BUF_DUMP();
      }
      
      // @TODO: add syntac highlighting to macros for literals
      // if (style_highlight_c(txt, buf, &buf_pos, &i))
      // { continue; }     
      
      bool skip_char_tmp = false;
      doc_color_code_escape_chars(txt, buf, &buf_pos, &txt_pos, &skip_char_tmp);
      
      if (!skip_char_tmp) { buf[buf_pos++] = txt[txt_pos++]; }
      else { txt_pos++; }
    }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  // attributes
  else if (txt[txt_pos] == '[' && txt[txt_pos +1] == '[')
  {
    BUF_DUMP();
    DOC_PF_COLOR(COL_ATTRIBUTE);
    while ( !(txt[txt_pos-2] == ']'  && txt[txt_pos-1] == ']' ) &&
             (txt[txt_pos]   != '\n' && txt[txt_pos]   != '\0')   )
    { 
      bool skip_char_tmp = false;
      doc_color_code_escape_chars(txt, buf, &buf_pos, &txt_pos, &skip_char_tmp);
      
      if (!skip_char_tmp) { buf[buf_pos++] = txt[txt_pos++]; }
      else { txt_pos++; }
    }
    BUF_DUMP();
    DOC_PF_COLOR(PF_WHITE);
    DOC_PF_STYLE_RESET();
  }
  
  return false;
}

void style_highlight_c_comment(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr) 
{
  // core_data_t* core_data = core_data_get();
  
  // buf_pos & i are macros for _ptr equivalent
  
  // starts with whitespace
  if (txt_pos <= 0 || !isspace(txt[txt_pos -1])) { return; }

  for (u32 s = 0; s < key_comment_len; ++s) // string
  {
    // @UNSURE: could use strncmp()
    bool equal = false;
    u32 len = strlen(key_comment[s]);
    u32 c;
    for (c = 0; c < len; ++c)             // char
    {
      equal = txt[txt_pos +c] == key_comment[s][c];
      if (!equal) { break; }
    }
    if (equal && isspace(txt[txt_pos +len]))
    {
      BUF_DUMP(); 
      DOC_PF_STYLE(PF_ITALIC, COL_COMMENT_HIGHLIGHT);
      
      while (c > 0) { buf[buf_pos++] = txt[txt_pos++]; c--; }   \
      
      BUF_DUMP(); 
      DOC_PF_STYLE(PF_ITALIC, COL_COMMENT);
      DOC_PF_STYLE(PF_DIM, COL_COMMENT);
      return;
    }
  }
}

void style_print_text_box(const char* box_indent, int box_width, int box_height, bool print_line_nr, const char* file, const int file_len, const char* name, int name_len)
{
  #define TEXT_MAX (file_len * 2)
  char* text = malloc(TEXT_MAX * sizeof(char));
  int text_len = 0;
  if (text == NULL) { DOC_PF_COLOR(PF_RED); DOC_PF("failed to print text\n"); DOC_PF_COLOR(PF_WHITE); return; } 

  // remove tabs
  {
    // STRCPY(text, file);
    for (int i = 0; i < file_len && text_len < TEXT_MAX; ++i)
    {
      if (file[i] == '\t') 
      {i++; text[text_len++] = '~'; text[text_len++] = '~'; }
      else { text[text_len++] = file[i]; }
    }
  }

  // print file
  {
    // DOC_PF("%s", file);
    #define SPACES_10 "          " 
    #define SPACES_50 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10
    char spaces[]  = SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50;
    u32 spaces_len = sizeof(spaces) / sizeof(spaces[0]);
    if (box_width >= spaces_len)
    { 
      DOC_PF_COLOR(PF_RED); DOC_PF("box_width: %d is max rn bc. spaces string\n", spaces_len); DOC_PF_COLOR(PF_WHITE); 
      box_width = spaces_len -1;
    } 
    int border_width = box_width -1 +2 +1 + (print_line_nr*4);
    int file_lines = 0;
    int lines = 0;

    DOC_PF("\r%s╭", box_indent); 
    for (int w = name_len +3; w > 0; --w) { DOC_PF("─"); } 
    DOC_PF("╮\n");
    DOC_PF("%s│ %s: │\n", box_indent, name);
    DOC_PF("%s├", box_indent);
    for (int w = 0; w < border_width; ++w) 
    { DOC_PF(w == name_len+3 ? "┴" : "─"); }
    if (border_width < name_len+3)
    {
      DOC_PF("┬");
      for (int w = (name_len +3) - border_width -1; w > 0; --w) { DOC_PF("─"); } 
      DOC_PF("╯\n");
    }
    else { DOC_PF("╮\n"); }

    int width = 0;
    int last_pos = 0;
    for (int t = 0; t < text_len; ++t)
    {
      if (text[t] == '\n')
      {
        lines++;
        if (lines >= box_height)  // check if max lines exceeded
        {
          DOC_PF("%s│ ...", box_indent); 
          DOC_PF("%.*s │", box_width +(print_line_nr*4) -3, spaces);
          DOC_PF("\n");
          break;
        }
        else
        {
          bool includes_newline = width <= box_width;
          file_lines += includes_newline;
          int real_width = width > box_width ? box_width : width;
          width = width < box_width ? width : box_width;
          if (includes_newline) 
          { 
            width -= file_lines <= 1 ? 1 : 2; 
          }

          // DOC_PF("width: %d, cd->w-w: %d\n", width, box_width -width);
          if (print_line_nr)
          { DOC_PF("%s│%03d│ %.*s", box_indent, file_lines > 999 ? 999 : file_lines, width, text +last_pos); }
          else
          { DOC_PF("%s│ %.*s", box_indent, width, text +last_pos); }
          DOC_PF("%.*s │", box_width -width, spaces);
          DOC_PF("\n");
          // if (includes_newline) { width += 2; }
          
          // last_pos += real_width +1;
          if (includes_newline) { last_pos = t +1; }
          else 
          { 
            // last_pos -= 1; 
            last_pos += real_width;
            t = last_pos -1;
          }
          // if (includes_newline) { last_pos += 2; }
          
          // last_pos += real_width + (includes_newline ? 2 : 0);
          // t = last_pos -1;
          // t = last_pos;
        }
        width = 0;
      }
      width++;
    }
    DOC_PF("%s╰", box_indent);
    for (int w = border_width; w > 0; --w) { DOC_PF("─"); }
    DOC_PF("╯\n");
    // P_V(lines);
  }

  FREE(text);
}

