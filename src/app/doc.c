#include "app/doc.h"
#include "app/file_io.h"
#include "app/style.h"
#include "app/core_data.h"

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
  int start;
  for (start = 1; txt[start] != '\0'; start++)
  {
    if (txt[start] == keyword[0] && 
        txt[start -1] == BORDER_CHAR)
    {
      found_one = true;
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
      while (txt[start] != '#') { start--; }
      int end = start +1;
      while (txt[end] != '#')   { end++; }

      start++;          // skip '#'
      char end_char = txt[end];
      txt[end] = '\0';  // shorten txt to section

      doc_print_section(txt + start, keyword, file);
      txt[end] = end_char;
      start = end +1;
      found = false;
      // break; 
    } 
  }
  FREE(txt);
  
  return found_one;
}

void doc_print_section(char* sec, const char* keyword, const char* file)
{
  PF_COLOR(PF_PURPLE);
  const int lne = 50;
  int i = strlen(keyword) + strlen(file) +1;
  PF("%s|%s ", file, keyword);
  while (i < lne) { PF("-"); i++; }
  PF("\n");
  
  PF_COLOR(PF_WHITE);
  doc_color_code_section(sec);
 
  PF_COLOR(PF_PURPLE);
  i = 0;
  while (i < lne) { PF("-"); i++; }
  PF("\n");
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

  #define DUMP_COLORED(n, col)                              \
      { BUF_DUMP();                                         \
      PF_COLOR(col);                                        \
      int j = 0;                                            \
      while (j < (n)) { buf[buf_pos++] = sec[i++]; j++; }   \
      BUF_DUMP();                                           \
      PF_COLOR(PF_WHITE); }

  for (int i = 0; i < len -1; ++i)
  {
    // -- functions --
    int j = 0;
    while(isalnum(sec[i +j])) 
    { j++; }
    if (sec[i +j] == '(')
    {
      BUF_DUMP();
      PF_COLOR(COL_FUNC);
      while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    
  

    // @TODO: 
    // style_highlight_c(&sec, &buf, &buf_pos, &i);

    // -- types --
    if (!isalnum(sec[i -1]) && sec[i -1] != '|')
    {
      if (sec[i +0] == 'v' && sec[i +1] == 'o' &&
          sec[i +2] == 'i' && sec[i +3] == 'd')
      { DUMP_COLORED(4, COL_TYPE); }
      if (sec[i +0] == 'i' && sec[i +1] == 'n' &&
          sec[i +2] == 't')
      { DUMP_COLORED(3, COL_TYPE); }
      if (sec[i +0] == 'f' && sec[i +1] == 'l' &&
          sec[i +2] == 'o' && sec[i +3] == 'a' &&
          sec[i +4] == 't')
      { DUMP_COLORED(5, COL_TYPE); }
      if (sec[i +0] == 'd' && sec[i +1] == 'o' &&
          sec[i +2] == 'u' && sec[i +3] == 'b' &&
          sec[i +4] == 'l' && sec[i +5] == 'e')
      { DUMP_COLORED(6, COL_TYPE); }
      if (sec[i +0] == 's' && sec[i +1] == 'h' &&
          sec[i +2] == 'o' && sec[i +3] == 'r' &&
          sec[i +4] == 't')
      { DUMP_COLORED(5, COL_TYPE); }
      if (sec[i +0] == 'l' && sec[i +1] == 'o' &&
          sec[i +2] == 'n' && sec[i +3] == 'g')
      { DUMP_COLORED(4, COL_TYPE); }
      if (sec[i +0] == 's' && sec[i +1] == 'i' &&
          sec[i +2] == 'z' && sec[i +3] == 'e' &&
          sec[i +4] == '_' && sec[i +5] == 't')
      { DUMP_COLORED(6, COL_TYPE); }
      if (sec[i +0] == 'u' && sec[i +1] == 'n' &&
          sec[i +2] == 's' && sec[i +3] == 'i' &&
          sec[i +4] == 'g' && sec[i +5] == 'n' &&
          sec[i +6] == 'e' && sec[i +7] == 'd')
      { DUMP_COLORED(8, COL_TYPE); }
      if (sec[i +0] == 'e' && sec[i +1] == 'n' &&
          sec[i +2] == 'u' && sec[i +3] == 'm')
      { DUMP_COLORED(4, COL_TYPE); }
      if (sec[i +0] == 's' && sec[i +1] == 't' &&
          sec[i +2] == 'r' && sec[i +3] == 'u' &&
          sec[i +4] == 'c' && sec[i +5] == 't')
      { DUMP_COLORED(6, COL_TYPE); }
      if (sec[i +0] == 'c' && sec[i +1] == 'h' &&
          sec[i +2] == 'a' && sec[i +3] == 'r')
      { DUMP_COLORED(4, COL_TYPE); }
      if (sec[i +0] == 'b' && sec[i +1] == 'o' &&
          sec[i +2] == 'o' && sec[i +3] == 'l')
      { DUMP_COLORED(4, COL_TYPE); }
      if (sec[i +0] == 'c' && sec[i +1] == 'o' &&
          sec[i +2] == 'n' && sec[i +3] == 's' &&
          sec[i +4] == 't')
      { DUMP_COLORED(5, COL_TYPE); }
    }
    // -- values --

    if (isdigit(sec[i]))                      // numbers
    {
      BUF_DUMP();
      PF_COLOR(COL_VALUE);
      int j = 0;
      while (isdigit(sec[i +j]) || sec[i +j] == '.' || sec[i +j] == 'f') { j++; }
      while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    if (sec[i +0] == 't' && sec[i +1] == 'r' &&
        sec[i +2] == 'u' && sec[i +3] == 'e')
    { DUMP_COLORED(4, COL_VALUE); }
    if (sec[i +0] == 'f' && sec[i +1] == 'a' &&
        sec[i +2] == 'l' && sec[i +3] == 's' &&
        sec[i +4] == 'e')
    { DUMP_COLORED(5, COL_VALUE); }
    if (sec[i +0] == 'N' && sec[i +1] == 'U' &&
        sec[i +2] == 'L' && sec[i +3] == 'L')
    { DUMP_COLORED(4, COL_VALUE); }
    if (sec[i] == '"')                        // strings
    {
      BUF_DUMP();
      PF_COLOR(COL_VALUE);
      buf[buf_pos++] = sec[i++];
      while (sec[i] != '"') { buf[buf_pos++] = sec[i++]; }
      buf[buf_pos++] = sec[i++];
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }
    if (sec[i] == '\'' && sec[i +2] == '\'')  // chars
    {
      BUF_DUMP();
      PF_COLOR(COL_VALUE);
      buf[buf_pos++] = sec[i++];
      buf[buf_pos++] = sec[i++];
      buf[buf_pos++] = sec[i++];
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }


    // -- warnings --
    if (sec[i] == '!')
    {
      BUF_DUMP();
      PF_STYLE_COL(PF_ITALIC, COL_WARNING);
      while (sec[i] != '\n') { buf[buf_pos++] = sec[i++]; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }

    // -- info --
    if (sec[i] == '~')
    {
      BUF_DUMP();
      PF_STYLE_COL(PF_ITALIC, COL_INFO);
      while (sec[i] != '\n') { buf[buf_pos++] = sec[i++]; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }

    // -- comments --
    if (sec[i] == '/' && sec[i +1] == '/')
    {
      BUF_DUMP();
      PF_STYLE(PF_ITALIC);
      PF_STYLE_COL(PF_DIM, COL_COMMENT);
      while (sec[i] != '\n') { buf[buf_pos++] = sec[i++]; }
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }

    // -- links --
    if (sec[i] == '?')
    {
      BUF_DUMP();
      i++; PF(" ");
      PF_STYLE(PF_DIM);
      PF_STYLE_COL(PF_ITALIC, COL_LINK);
      while (sec[i] != '?') { buf[buf_pos++] = sec[i++]; }
      i++; PF(" ");
      BUF_DUMP();
      PF_COLOR(PF_WHITE);
    }

    // -- example --
    if (sec[i +0] == 'e' && sec[i +1] == 'x' &&
        sec[i +2] == 'a' && sec[i +3] == 'm' &&
        sec[i +4] == 'p' && sec[i +5] == 'l' &&
        sec[i +6] == 'e' && sec[i +7] == ':')
    { DUMP_COLORED(8, COL_EXAMPLE); }

    // -- copy from sec --
    buf[buf_pos++] = sec[i];

    // -- tags --
    if (sec[i] == '|' && isspace(sec[i+1]) && in_tag)
    { 
      BUF_DUMP();
      PF_COLOR(PF_WHITE);;
      in_tag = false;
      continue;
    }
    else if (sec[i +1] == '|')
    {
      BUF_DUMP();
      PF_STYLE_COL(PF_UNDERLINE, COL_TAG);;
      in_tag = true;
      continue;
    }

  }
  buf[buf_pos] = '\0';
  PF("%s", buf);
  PF("\n");
  PF("\n");   // twice bc. cutting off in for (len -1)

  #undef BUF_DUMP
  #undef DUMP_TYPE

}



