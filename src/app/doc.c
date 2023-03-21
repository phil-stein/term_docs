#include "app/doc.h"
#include "app/file_io.h"
#include "app/style.h"
#include "app/core_data.h"
#include "global/global.h"

#include <ctype.h>

#include <direct.h>
#include <dirent/dirent.h>



void doc_search_dir(const char* dir_path, const char* keyword, int* n)
{
  char path[256];
  struct dirent* dp;
  DIR* dir = opendir(dir_path);
  // unable to open directory stream
  if (!dir) { return; }

  // recursively read the directory and its sub-directories
  while ((dp = readdir(dir)) != NULL)
  {
    // check that the file currently read isn't a directory
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
    {
      // check_file(dp->d_name, strlen(dp->d_name), dir_path);
      if (dp->d_name[dp->d_namlen -6] == '.' &&
          dp->d_name[dp->d_namlen -5] == 's' &&
          dp->d_name[dp->d_namlen -4] == 'h' &&
          dp->d_name[dp->d_namlen -3] == 'e' &&
          dp->d_name[dp->d_namlen -2] == 'e' &&
          dp->d_name[dp->d_namlen -1] == 't' )
      {
        char buf[300];
        SPRINTF(300, buf, "%s%s", dir_path, dp->d_name);
        if (doc_search_section(buf, dp->d_name, keyword)) 
        { 
          *n += 1;
        }
      }

      // construct new path from our base path
      strcpy(path, dir_path);
      strcat(path, "\\");
      strcat(path, dp->d_name);

      doc_search_dir(path, keyword, n); // search recursively
    }
  }

  // close the stream
  closedir(dir);
}

bool doc_search_section(const char* path, const char* file, const char* keyword)
{
  // const char* path = "sheets/c.sheet";
  if (!check_file_exists(path)) { return false; }
  char* txt = read_text_file(path);

  // @TODO: see if better way to do this
  bool found_one = false;
  bool found = false;
  u32 found_count = 0;
  int start;
  for (start = 1; txt[start] != '\0'; start++)
  {
    if (txt[start] == keyword[0] && 
        txt[start -1] == BORDER_CHAR)
    {
      found = true;
      int j = 0;
      for (j = 0; keyword[j] != '\0'; j++)
      {
        if (txt[start +j] != keyword[j])
        { found = false; break; }
      }
      if (txt[start +j] != BORDER_CHAR) 
      { found = false; }
    }
    if (found) 
    { 
      // -- section start & end --
      while (txt[start] != '#' || txt[start -1] == '\\') { start--; }
      int end = start +1;
      while (txt[end] != '#' || txt[end -1] == '\\')   { end++; }

      start++;          // skip '#'
      char end_char = txt[end];
      txt[end] = '\0';  // shorten txt to section

      doc_print_section(txt + start, keyword, file);
      txt[end] = end_char;
      start = end +1;
      found_one = true;
      found = false;
      found_count++;
    }
  }
  FREE(txt);

  // print ending line
  if (found_count > 0)
  {
    int w, h; io_util_get_console_size_win(&w, &h);
    w = w > MAX_LINE_WIDTH ? MAX_LINE_WIDTH : w;;
    
    PF_COLOR(PF_PURPLE);
    int i = 0;
    while( i < w -1) { _PF("-"); i++; }
    
    PF("\n");
    PF_COLOR(PF_WHITE);
  }

  return found_one;
}

void doc_print_section(char* sec, const char* keyword, const char* file)
{
  int w, h; io_util_get_console_size_win(&w, &h);
  w = w > MAX_LINE_WIDTH ? MAX_LINE_WIDTH : w;;

  PF_COLOR(PF_PURPLE);
  PF("%s|%s ", file, keyword); 
  int i = strlen(keyword) + strlen(file) +2;
  while( i < w -1) { _PF("-"); i++; }
  PF("\n");
  
  PF_COLOR(PF_WHITE);
  doc_color_code_section(sec);
 
  PF_COLOR(PF_WHITE);
}

void doc_color_code_section(char* sec)
{
  core_data_t* core_data = core_data_get();
  // @NOTE: no syntax highlighting
  if (!core_data->style_act)
  {
    PF("%s\n", sec);
    return;
  }

  // @NOTE: syntax highlighting
  int  len = strlen(sec);
  char buf[512];
  int  buf_pos = 0;
  bool in_tag = false;
  
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
      while (j < (n)) { buf[buf_pos++] = sec[i++]; j++; }   \
      BUF_DUMP();                                           \
      PF_COLOR(PF_WHITE); }

  for (int i = 0; i < len -1; ++i)
  {    
    // highlight all c syntax
    style_highlight_c(sec, buf, &buf_pos, &i);
    

    bool skip_char = false;

    // -- warnings --
    if (sec[i] == '!' && sec[i -1] != '\\')
    {
      // // skip escaped char, not doing that anymore
      // if (sec[i -1] == '\\') { buf_pos -= 2; BUF_DUMP(); i++; } 
      // else { BUF_DUMP(); }
      BUF_DUMP();
      PF_STYLE(PF_ITALIC, COL_WARNING);
      buf[buf_pos++] = sec[i++];
      while (sec[i] != '\n' && sec[i] != '!') { buf[buf_pos++] = sec[i++]; }
      if (sec[i] == '!') { i++; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    else if (sec[i] == '!' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- info --
    if (sec[i] == '~' && sec[i -1] != '\\')
    {
      // // skip escaped char, not doing that anymore
      // if (sec[i -1] == '\\') { buf_pos -= 2; BUF_DUMP(); i++; } 
      // else { BUF_DUMP(); }
      BUF_DUMP();
      PF_STYLE(PF_ITALIC, COL_INFO);
      buf[buf_pos++] = sec[i++];
      while (sec[i] != '\n' && sec[i] != '~') { buf[buf_pos++] = sec[i++]; }
      if (sec[i] == '~') { i++; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    else if (sec[i] == '~' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- links --
    if (sec[i] == '?' && sec[i -1] != '\\')
    {
      // // skip escaped char, not doing that anymore
      // if (sec[i -1] == '\\') { buf_pos -= 2; BUF_DUMP(); i++; } 
      // else { BUF_DUMP(); }
      BUF_DUMP();
      buf[buf_pos++] = sec[i++];
      while (sec[i] != '\n' && sec[i] != '?') { buf[buf_pos++] = sec[i++]; }
      if (sec[i] == '?') { i++; }
      PF_STYLE(PF_DIM, COL_LINK);
      PF_STYLE(PF_ITALIC, COL_LINK);
      // i++; PF(" ");
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    else if (sec[i] == '?' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- example --
    if (sec[i +0] == 'e' && sec[i +1] == 'x' &&
        sec[i +2] == 'a' && sec[i +3] == 'm' &&
        sec[i +4] == 'p' && sec[i +5] == 'l' &&
        sec[i +6] == 'e' && sec[i +7] == ':')
    { DUMP_COLORED(8, COL_EXAMPLE); }

    // -- escaped --
    if (sec[i] == '\\' && sec[i +1] == '#')
    {
      BUF_DUMP();
      // i++;
      // buf_pos--;
      skip_char = true;
    }

    // -- copy from sec --
    if (!skip_char) { buf[buf_pos++] = sec[i]; }

    // -- tags --
    // if (sec[i] == '|' && isspace(sec[i+1]) && in_tag) // end tag
    // { 
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    //   in_tag = false;
    //   // continue;
    // }
    // else if (sec[i +1] == '|')  // start tag
    // {
    //   BUF_DUMP();
    //   PF_STYLE_COL(PF_UNDERLINE, COL_TAG);;
    //   in_tag = true;
    //   // continue;
    // }
    if (!(sec[i] == '|' && isspace(sec[i+1]) && in_tag) && sec[i +1] == '|')
    {
      BUF_DUMP();
      PF_STYLE(PF_UNDERLINE, COL_TAG);
      
      while (!(sec[i] == '|' && isspace(sec[i+1])) && i < len)
      {
        if (sec[i] == '\n') { i++; continue; }
        buf[buf_pos++] = sec[i++];
      }
      if (sec[i] != '\n' || sec[i] == '\0') { buf[buf_pos++] = sec[i++]; }
      
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }

  }
  buf[buf_pos] = '\0';
  PF("%s", buf);
  PF("\n");
  PF("\n");   // twice bc. cutting off in for (len -1)

  #undef BUF_DUMP
  #undef DUMP_TYPE

}

// old highlight system
    // if (isspace(sec[i -1]) && sec[i -1] != '|') //  && !in_tag)  // !isalnum(sec[i -1])
    // {

    //   // -- types --
    //   for (u32 s = 0; s < key_types_len; ++s) // string
    //   {
    //     // @UNSURE: could use strcmp()
    //     bool equal = false;
    //     u32 len = strlen(key_types[s]);
    //     u32 c;
    //     for (c = 0; c < len; ++c)             // char
    //     {
    //       equal = sec[i +c] == key_types[s][c];
    //       if (!equal) { break; }
    //     }
    //     if (equal && TYPE_END(sec[i +len]))
    //     {
    //       DUMP_COLORED(len, COL_TYPE);
    //       break;
    //     }
    //   }

    //   // -- flow control --
    //   for (u32 s = 0; s < key_flow_ctrl_len; ++s) // string
    //   {
    //     // @UNSURE: could use strcmp()
    //     bool equal = false;
    //     u32 len = strlen(key_flow_ctrl[s]);
    //     u32 c;
    //     for (c = 0; c < len; ++c)             // char
    //     {
    //       equal = sec[i +c] == key_flow_ctrl[s][c];
    //       if (!equal) { break; }
    //     }
    //     if (equal && FLOW_CTRL_END(sec[i +len]))
    //     {
    //       DUMP_COLORED(len, COL_TYPE);
    //       break;
    //     }
    //   }
    // }

    // // -- functions --
    // int j = 0;
    // while(isalnum(sec[i +j]) || sec[i +j] == '_') 
    // { j++; }
    // if (sec[i +j] == '(')
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_FUNC);
    //   while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }

    // // -- values --

    // if (isdigit(sec[i]) && !isalpha(sec[i -1]) && 
    //     (!isalpha(sec[i +1]) || sec[i +1] == 'f') && 
    //     !in_tag)                                      // numbers
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   int j = 0;
    //   while (isdigit(sec[i +j]) || sec[i +j] == '.' || sec[i +j] == 'f') { j++; }
    //   while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }
    // if (sec[i +0] == 't' && sec[i +1] == 'r' &&     // value symbols
    //     sec[i +2] == 'u' && sec[i +3] == 'e')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (sec[i +0] == 'f' && sec[i +1] == 'a' &&
    //     sec[i +2] == 'l' && sec[i +3] == 's' &&
    //     sec[i +4] == 'e')
    // { DUMP_COLORED(5, COL_VALUE); }
    // if (sec[i +0] == 'N' && sec[i +1] == 'U' &&
    //     sec[i +2] == 'L' && sec[i +3] == 'L')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (sec[i] == '"')                        // strings
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   buf[buf_pos++] = sec[i++];
    //   while (sec[i] != '"') { buf[buf_pos++] = sec[i++]; }
    //   buf[buf_pos++] = sec[i++];
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }
    // if (sec[i] == '\'' && sec[i +2] == '\'')  // chars
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   buf[buf_pos++] = sec[i++];
    //   buf[buf_pos++] = sec[i++];
    //   buf[buf_pos++] = sec[i++];
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }



// old keyword highlight system
//
      // if (sec[i +0] == 'v' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'i' && sec[i +3] == 'd' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'i' && sec[i +1] == 'n' &&
      //     sec[i +2] == 't' && TYPE_END(sec[i +3]))
      // { DUMP_COLORED(3, COL_TYPE); }
      // if (sec[i +0] == 'f' && sec[i +1] == 'l' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'a' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 'd' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'u' && sec[i +3] == 'b' &&
      //     sec[i +4] == 'l' && sec[i +5] == 'e' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 'h' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'r' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 'l' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'n' && sec[i +3] == 'g' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 'i' &&
      //     sec[i +2] == 'z' && sec[i +3] == 'e' &&
      //     sec[i +4] == '_' && sec[i +5] == 't' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 'u' && sec[i +1] == 'n' &&
      //     sec[i +2] == 's' && sec[i +3] == 'i' &&
      //     sec[i +4] == 'g' && sec[i +5] == 'n' &&
      //     sec[i +6] == 'e' && sec[i +7] == 'd' && 
      //     TYPE_END(sec[i +8]))
      // { DUMP_COLORED(8, COL_TYPE); }
      // if (sec[i +0] == 'e' && sec[i +1] == 'n' &&
      //     sec[i +2] == 'u' && sec[i +3] == 'm' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 't' &&
      //     sec[i +2] == 'r' && sec[i +3] == 'u' &&
      //     sec[i +4] == 'c' && sec[i +5] == 't' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 'c' && sec[i +1] == 'h' &&
      //     sec[i +2] == 'a' && sec[i +3] == 'r' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'b' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'l' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'c' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'n' && sec[i +3] == 's' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 't' && sec[i +1] == 'y' &&
      //     sec[i +2] == 'p' && sec[i +3] == 'e' &&
      //     sec[i +4] == 'd' && sec[i +5] == 'e' &&
      //     sec[i +6] == 'f' && TYPE_END(sec[i +7]))
      // { DUMP_COLORED(7, COL_TYPE); }
 



