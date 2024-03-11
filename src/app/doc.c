#include "app/doc.h"
#include "app/file_io.h"
#include "app/style.h"
#include "app/core_data.h"
#include "global/global.h"
#include "global/global_print.h"

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>

u32 cur_pf_color = PF_WHITE; 
u32 cur_pf_style = PF_NORMAL;

u32 default_pf_color = PF_WHITE; 
u32 default_pf_style = PF_NORMAL;

#define CUR_FILE_PATH_MAX 256
char cur_file_path[CUR_FILE_PATH_MAX];

// @TODO: make keword an array and check all keywords are in section
void doc_search_dir(const char* dir_path, const char** keywords, int keywords_len, int* n)
{
  char path[256];
  struct dirent* dp;
  DIR* dir = opendir(dir_path);
  // unable to open directory stream
  if (!dir) 
  {
    return; 
  }
  // P_STR(dir_path);
  

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
        // add slash if missing
        char* slash = dir_path[strlen(dir_path) -1] == '/'  ? "" :
                      dir_path[strlen(dir_path) -1] == '\\' ? "" : "\\";
        SPRINTF(300, buf, "%s%s%s", dir_path, slash, dp->d_name);
        // P_STR(buf);
        if (doc_search_file(buf, dp->d_name, keywords, keywords_len)) 
        { 
          *n += 1;
        }
      }
      

      // construct new path from our base path
      strcpy(path, dir_path);
      path[strlen(path) -1] = '\0';
      strcat(path, "\\");
      strcat(path, dp->d_name);
      
      doc_search_dir(path, keywords, keywords_len, n); // search recursively
    }
  }

  // close the stream
  closedir(dir);
}

bool doc_search_file(const char* path, const char* file, const char** keywords, int keywords_len)
{
  if (!check_file_exists(path)) { return false; }
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);

  core_data_t* core_data = core_data_get();
  
  // P_STR(path);
  
  int sections_found_count = 0; // found sections
  int start;
  for (start = 0; txt[start] != '\0'; ++start)
  {
    // with this it cant find calloc, idk
    // // @UNSURE: this loop wasnt here but still working why ???
    // // @BUGG: txt[start-1] start might be 0
    // while (start < txt_len && (txt[start] != '#' || txt[start -1] == '\\')) 
    // { start++; }
    
    int end = start +1;
    while (end < txt_len && (txt[end] != '#' || txt[end -1] == '\\')) 
    { end++; }
    if (txt[end] != '#') 
    { break; }

    // go through section once for each keyword
    int keyword_found_count = 0;
    for (int keyword_idx = 0; keyword_idx < keywords_len; ++keyword_idx)
    {
      const char* keyword = keywords[keyword_idx];

      bool keyword_found = false;
      // search for keyword
      for (int section = start; section < end; ++section)
      {
        int  keyword_pos = 0;
      
        // escaped #, skip the '\'
        if (txt[section] == '\\' && txt[section +1] == '#') 
        { section++; }

        // if the first char in sectin fits keyword's first char
        // and the char before is |, because |<tag>| 
         // loop over the next chars to see if it is the keyword
        if (txt[section -1] == BORDER_CHAR && txt[section] == keyword[keyword_pos])
        {
          // assume true incase strlen(keyword) == 1
          keyword_found = true;
          // start at the first char, checked in the if stateent above
          // and check if the next chars fit aswell
          int tag_pos = section;
          for (; tag_pos < section + strlen(keyword) && tag_pos < txt_len ; ++tag_pos)
          {
              if (txt[tag_pos] == '|' || txt[tag_pos] != keyword[keyword_pos]) { keyword_found = false; break; }
              // section++;
              keyword_pos++;
          }
          
          // check if tag ends in | 
          if (txt[tag_pos] != BORDER_CHAR) { keyword_found = false; }
          
          if (keyword_found) 
          {
            // // PF("in loop: "); P_BOOL(keyword_found);
            // for (int c = 0; c  < strlen(keyword); ++c)
            // { PF("%c", txt[section +c]); }
            // PF("\n");
  
            keyword_found_count++; 
            
            // has to be done here and in case not found
            section = tag_pos;
            break; 
          }
          // has to be done here and in case found
          section = tag_pos;
        }
      }
    }
    
    // print if all keywords found
    if (keyword_found_count == keywords_len)
    {
      int lines_count = 1;  // idk has to be 1
      for (int i = 0; i < txt_len && i <= start; ++i)
      { if (txt[i] == '\n') { lines_count++;} }
      // print location
      if (core_data->print_loc_act)
      {
        DOC_PF_COLOR(PF_CYAN);
        PF("location");
        DOC_PF_COLOR(PF_WHITE);
        PF(":\n -> ");
        DOC_PF_COLOR(PF_CYAN);
        PF("file"); 
        DOC_PF_COLOR(PF_WHITE);
        PF(": %s\n -> ", path);
        DOC_PF_COLOR(PF_CYAN);
        PF("line");
        DOC_PF_COLOR(PF_WHITE);
        PF(": %d\n", lines_count);
      }
      STRCPY(cur_file_path, path);
      doc_print_section(txt + start, end - start, keywords, keywords_len, file, lines_count);
      sections_found_count++;
    }

    // skip to next section
    start = end;
  }

  FREE(txt);

  return sections_found_count > 0;
}


void doc_print_section(char* sec, int sec_len, const char** keywords, int keywords_len, const char* file, const int line)
{
  // core_data_t* core_data = core_data_get();
  
  char _keywords[128];
  for (int i = 0; i < keywords_len; ++i)
  { strcat(_keywords, " "); strcat(_keywords, keywords[i]); }

  char title[256];
  SPRINTF(64, title, "    %s[%d] |%s    ", file, line, _keywords);

  style_draw_title(title);
  
  doc_color_code_section(sec, sec_len);
   
  style_draw_line();
}

  
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
    SET_COLOR(col);                                       \
    int j = 0;                                            \
    while (j < (n)) { buf[buf_pos++] = sec[i++]; j++; }   \
    BUF_DUMP();                                           \
    SET_COLOR(PF_WHITE);                                  \
    DOC_PF_MODE_RESET();                                  \
    }

#define SET_STYLE(m, c) if (core_data->style_act) { PF_STYLE((m), (c)); cur_pf_style = (m); cur_pf_color = (c); }
#define SET_COLOR(c)    if (core_data->style_act) { PF_COLOR((c)); cur_pf_color = (c); }

#define RESET_STYLE() if (core_data->style_act) { PF_MODE_RESET(); RESET_DEFAULT_STYLE(); cur_pf_style = PF_NORMAL; cur_pf_color = PF_WHITE; }

#define SET_DEFAULT_STYLE(m, c)   if (core_data->style_act) { default_pf_style = (m); default_pf_color = (c); }
#define RESET_DEFAULT_STYLE()     if (core_data->style_act) { default_pf_style = PF_NORMAL; default_pf_color = PF_WHITE; }

void doc_color_code_section(char* sec, int len)
{
  core_data_t* core_data = core_data_get();
  // @NOTE: no syntax highlighting
  // if (!core_data->style_act)
  // {
  //   // PF("%s\n", sec);
  //   u32 i = 0;
  //   while (sec[i] != '\0')
  //   {
  //     if (sec[i] == '\r') { PF("CR"); }
  //     if (sec[i] == '\n') { PF("*"); }
  //     PF("%c", sec[i++]);
  //   }
  //   return;
  // }

  // @NOTE: syntax highlighting
  // int   len = strlen(sec);
  char* buf = malloc(len + 12 * sizeof(char));  // need to malloc in case long sec
  int   buf_pos = 0;
  // bool in_tag = false;
  bool syntax = true; // true: highligh c syntax, false: dont

  for (int i = 0; i < len -1; ++i)
  {    
    // highlight all c syntax
    if (syntax)
    { 
      if (style_highlight_c(sec, buf, &buf_pos, &i))
      { continue; }
    }
    
    bool skip_char = false;

    // -- warnings --
    if (sec[i] == '!' && sec[i -1] != '\\')
    {
      BUF_DUMP();
      SET_STYLE(PF_ITALIC, COL_WARNING);
      SET_DEFAULT_STYLE(PF_ITALIC, COL_WARNING);
      // buf[buf_pos++] = sec[i++]; 
      i++;
      _PF(core_data->warning_icon);
      while (sec[i] != '\n' && sec[i] != '!') 
      { 
        bool skip_char_tmp = false;
        doc_color_code_escape_chars(sec, buf, &buf_pos, &i, &skip_char_tmp);
        doc_color_code_color_codes(sec, len, buf, &buf_pos, &i, &skip_char_tmp, &syntax);
        if (!skip_char_tmp) { buf[buf_pos++] = sec[i++]; }
        else { i++; }
      }
      if (sec[i] == '!') { i++; }
      BUF_DUMP();
      RESET_DEFAULT_STYLE();
      // SET_COLOR(PF_WHITE);
      // DOC_PF_MODE_RESET();
      RESET_STYLE();
    }
    else if (sec[i] == '!' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- info --
    if (sec[i] == '~' && sec[i -1] != '\\')
    {
      BUF_DUMP();
      SET_STYLE(PF_ITALIC, COL_INFO);
      SET_DEFAULT_STYLE(PF_ITALIC, COL_INFO);
      // buf[buf_pos++] = sec[i++];
      i++;
      _PF(core_data->info_icon);
      while (sec[i] != '\n' && sec[i] != '~') 
      {
        bool skip_char_tmp = false;
        doc_color_code_escape_chars(sec, buf, &buf_pos, &i, &skip_char_tmp);
        doc_color_code_color_codes(sec, len, buf, &buf_pos, &i, &skip_char_tmp, &syntax);
        if (!skip_char_tmp) { buf[buf_pos++] = sec[i++]; }
        else { i++; }
      }
      if (sec[i] == '~') { i++; }
      BUF_DUMP();
      RESET_DEFAULT_STYLE();
      // SET_COLOR(PF_WHITE);
      // DOC_PF_MODE_RESET();
      RESET_STYLE();
    }
    else if (sec[i] == '~' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- links --
    if (sec[i] == '?' && sec[i -1] != '\\')
    {
      BUF_DUMP();
      SET_STYLE(PF_DIM, COL_LINK);
      SET_STYLE(PF_ITALIC, COL_LINK);
      // buf[buf_pos++] = sec[i++];
      i++;
      _PF(core_data->link_icon);
      while ((sec[i] != '\n' && sec[i] != '\0') && 
             (sec[i] != '?' || sec[i -1] == '\\'))
      {
        // escaped ?
        if (sec[i] == '\\' && sec[i +1] == '?') { i++; continue; }
        buf[buf_pos++] = sec[i++]; 
      }
      if (sec[i] == '?') { i++; }
      BUF_DUMP();
      RESET_DEFAULT_STYLE();
      // SET_COLOR(PF_WHITE);
      // DOC_PF_MODE_RESET();
      RESET_STYLE();
    }
    else if (sec[i] == '?' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- example --
    if (sec[i +0] == 'e' && sec[i +1] == 'x' &&
        sec[i +2] == 'a' && sec[i +3] == 'm' &&
        sec[i +4] == 'p' && sec[i +5] == 'l' &&
        sec[i +6] == 'e' && sec[i +7] == ':')
    { DUMP_COLORED(8, COL_EXAMPLE); }

    // -- escaped --
    doc_color_code_escape_chars(sec, buf, &buf_pos, &i, &skip_char);

    // -- color code --
    doc_color_code_color_codes(sec, len, buf, &buf_pos, &i, &skip_char, &syntax);


    // -- copy from sec --
    if (!skip_char) { buf[buf_pos++] = sec[i]; }

    // -- tags -- 
    if (!(sec[i] == '|' && isspace(sec[i+1])) && sec[i +1] == '|')
    {
      BUF_DUMP();
      SET_STYLE(PF_UNDERLINE, COL_TAG);
      
      while (!(sec[i] == '|' && isspace(sec[i+1])) && i < len)
      {
        if (sec[i] == '\n') { i++; continue; }
        // skip escaped #
        if (sec[i] == '\\' && sec[i +1] == '#') { i++; continue; }
        
        buf[buf_pos++] = sec[i++];
      }
      if (sec[i] != '\n' || sec[i] == '\0') { buf[buf_pos++] = sec[i++]; }
      
      BUF_DUMP();
      RESET_DEFAULT_STYLE();
      // SET_COLOR(PF_WHITE);
      // DOC_PF_MODE_RESET();
      RESET_STYLE();
    }

  }
  buf[buf_pos] = '\0';
  PF("%s", buf);
  PF("\n");
  PF("\n");   // twice bc. cutting off in for (len -1)
  free(buf);

}

void doc_color_code_escape_chars(char* sec, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr)
{
  #define buf_pos   (*buf_pos_ptr)
  #define i         (*i_ptr)
  #define skip_char (*skip_char_ptr)
  
  // core_data_t* core_data = core_data_get();
  
  if (sec[i] == '\\' && sec[i +1] == '\\' && sec[i+2] == '#')
  {
    buf[buf_pos++] = sec[i +2]; // put # in buf
    BUF_DUMP();                 // output buf
    skip_char = true;           // ignore '\\'
    i+=2;                       // set i after #
  }
  else if (sec[i] == '\\' && sec[i+1] == '#')
  {
    BUF_DUMP();
    skip_char = true; // skip '\'
  }
  else if (sec[i] == '\\' && ( sec[i+1] == '|' || sec[i+1] == '~' || 
                               sec[i+1] == '!' || sec[i+1] == '?'))
  {
    buf[buf_pos++] = sec[i +1]; // put | in buf
    BUF_DUMP();                 // output buf
    skip_char = true;           // ignore '\'
    i++;                        // set i after |
  }
  
  #undef buf_pos
  #undef i     
  #undef skip_char
}

void doc_color_code_color_codes(char* sec, int sec_len, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr, bool* syntax)
{
  #define buf_pos   (*buf_pos_ptr)
  #define i         (*i_ptr)
  #define skip_char (*skip_char_ptr)
  
  core_data_t* core_data = core_data_get();

  if (sec[i] == '$' && sec[i+1] == '$')                             // reset to normal
  { 
    BUF_DUMP(); RESET_STYLE();  SET_STYLE(default_pf_style, default_pf_color); 
    (*syntax) = true; skip_char = true; i++; 
  }
  // white
  if (i+6 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'w' || sec[i+1] == 'W') && 
      (sec[i+2] == 'h' || sec[i+2] == 'H') && 
      (sec[i+3] == 'i' || sec[i+3] == 'I') && 
      (sec[i+4] == 't' || sec[i+4] == 'T') && 
      (sec[i+5] == 'e' || sec[i+5] == 'E') && 
       sec[i+6] == '$' )   // set color to white
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_WHITE); skip_char = true; i += 6; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'w' || sec[i+1] == 'W') && 
       sec[i+2] == '$' )   // set color to white
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_WHITE); skip_char = true; i += 2; }
  // red
  if (i+4 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'r' || sec[i+1] == 'R') &&                         
      (sec[i+2] == 'e' || sec[i+2] == 'E') && 
      (sec[i+3] == 'd' || sec[i+3] == 'D') && 
       sec[i+4] == '$')    // set color to red
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_RED); skip_char = true; i += 4; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'r' || sec[i+1] == 'R') &&                         
       sec[i+2] == '$')    // set color to red
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_RED); skip_char = true; i += 2; }
  // black 
  if (i+6 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'b' || sec[i+1] == 'B') && 
      (sec[i+2] == 'l' || sec[i+2] == 'L') && 
      (sec[i+3] == 'a' || sec[i+3] == 'A') && 
      (sec[i+4] == 'c' || sec[i+4] == 'C') && 
      (sec[i+5] == 'k' || sec[i+5] == 'K') && 
       sec[i+6] == '$' )   // set color to black
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLACK); skip_char = true; i += 6; }
  // green
  if (i+6 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'g' || sec[i+1] == 'G') && 
      (sec[i+2] == 'r' || sec[i+2] == 'R') && 
      (sec[i+3] == 'e' || sec[i+3] == 'E') && 
      (sec[i+4] == 'e' || sec[i+4] == 'E') && 
      (sec[i+5] == 'n' || sec[i+5] == 'N') && 
       sec[i+6] == '$' )   // set color to green
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_GREEN); skip_char = true; i += 6; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'g' || sec[i+1] == 'G') && 
       sec[i+2] == '$' )   // set color to green
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_GREEN); skip_char = true; i += 2; }
  // yellow
  if (i+7 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'y' || sec[i+1] == 'Y') && 
      (sec[i+2] == 'e' || sec[i+2] == 'E') && 
      (sec[i+3] == 'l' || sec[i+3] == 'L') && 
      (sec[i+4] == 'l' || sec[i+4] == 'L') && 
      (sec[i+5] == 'o' || sec[i+5] == 'O') && 
      (sec[i+6] == 'w' || sec[i+6] == 'W') && 
       sec[i+7] == '$' )   // set color to yellow
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_YELLOW); skip_char = true; i += 7; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'y' || sec[i+1] == 'Y') && 
       sec[i+2] == '$' )   // set color to yellow
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_YELLOW); skip_char = true; i += 2; }
  // blue
  if (i+5 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'b' || sec[i+1] == 'B') && 
      (sec[i+2] == 'l' || sec[i+2] == 'L') && 
      (sec[i+3] == 'u' || sec[i+3] == 'U') && 
      (sec[i+4] == 'e' || sec[i+4] == 'E') && 
       sec[i+5] == '$' )   // set color to blue
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLUE); skip_char = true; i += 5; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'b' || sec[i+1] == 'B') && 
       sec[i+2] == '$' )   // set color to blue
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLUE); skip_char = true; i += 2; }
  // purple
  if (i+7 < sec_len   &&
       sec[i]   == '$' && 
      (sec[i+1] == 'p' || sec[i+1] == 'P') && 
      (sec[i+2] == 'u' || sec[i+2] == 'U') && 
      (sec[i+3] == 'r' || sec[i+3] == 'R') && 
      (sec[i+4] == 'p' || sec[i+4] == 'P') && 
      (sec[i+5] == 'l' || sec[i+5] == 'L') && 
      (sec[i+6] == 'e' || sec[i+6] == 'E') && 
       sec[i+7] == '$' )   // set color to purple
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_PURPLE); skip_char = true; i += 7; }
  if (i+2 < sec_len   &&
       sec[i]   == '$' && 
      (sec[i+1] == 'p' || sec[i+1] == 'P') && 
       sec[i+2] == '$' )   // set color to purple
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_PURPLE); skip_char = true; i += 2; }
  // cyan
  if (i+5 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'c' || sec[i+1] == 'C') && 
      (sec[i+2] == 'y' || sec[i+2] == 'Y') && 
      (sec[i+3] == 'a' || sec[i+3] == 'A') && 
      (sec[i+4] == 'n' || sec[i+4] == 'N') && 
       sec[i+5] == '$' )   // set color to cyan
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_CYAN); skip_char = true; i += 5; }
  if (i+2 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 'c' || sec[i+1] == 'C') && 
       sec[i+2] == '$' )   // set color to cyan
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_CYAN); skip_char = true; i += 2; }

  // -- modes -- 

  // italic
  if (i+7 < sec_len   &&
       sec[i]   == '$' && 
      (sec[i+1] == 'i' || sec[i+1] == 'I') && 
      (sec[i+2] == 't' || sec[i+2] == 'T') && 
      (sec[i+3] == 'a' || sec[i+3] == 'A') && 
      (sec[i+4] == 'l' || sec[i+4] == 'L') && 
      (sec[i+5] == 'i' || sec[i+5] == 'I') && 
      (sec[i+6] == 'c' || sec[i+6] == 'C') && 
       sec[i+7] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_ITALIC, cur_pf_color); skip_char = true; i += 7; }
  if (i+2 < sec_len   &&
       sec[i]   == '$' && 
       sec[i+1] == '/' && 
       sec[i+2] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_ITALIC, cur_pf_color); skip_char = true; i += 2; }
  // dim
  if (i+4 < sec_len   &&
       sec[i]   == '$' && 
      (sec[i+1] == 'd' || sec[i+1] == 'D') && 
      (sec[i+2] == 'i' || sec[i+2] == 'I') && 
      (sec[i+3] == 'm' || sec[i+3] == 'M') && 
       sec[i+4] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_DIM, cur_pf_color); skip_char = true; i += 4; }
  // underline
  if (i+10 < sec_len   &&
       sec[i]   == '$' && 
      (sec[i+ 1] == 'u' || sec[i+1] == 'U') && 
      (sec[i+ 2] == 'n' || sec[i+2] == 'N') && 
      (sec[i+ 3] == 'd' || sec[i+3] == 'D') && 
      (sec[i+ 4] == 'e' || sec[i+4] == 'E') && 
      (sec[i+ 5] == 'r' || sec[i+5] == 'R') && 
      (sec[i+ 6] == 'l' || sec[i+6] == 'L') && 
      (sec[i+ 7] == 'i' || sec[i+7] == 'I') && 
      (sec[i+ 8] == 'n' || sec[i+8] == 'N') && 
      (sec[i+ 9] == 'e' || sec[i+9] == 'E') && 
       sec[i+10] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_UNDERLINE, cur_pf_color);  skip_char = true; i += 10; }
  if (i+2 < sec_len   &&
       sec[i]   == '$' && 
       sec[i+1] == '_' && 
       sec[i+2] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_UNDERLINE, cur_pf_color);  skip_char = true; i += 2; }


  // -- info / warning / tag / link --
  if (i+2 < sec_len    &&
      sec[i]   == '$' && 
      sec[i+1] == '~' &&                         
      sec[i+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_INFO, COL_INFO);  
    (*syntax) = false; skip_char = true; i += 2; 
  }
  if (i+2 < sec_len    &&
      sec[i]   == '$' && 
      sec[i+1] == '!' &&                         
      sec[i+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_WARNING, COL_WARNING);  
    (*syntax) = false; skip_char = true; i += 2; 
  }
  if (i+2 < sec_len    &&
      sec[i]   == '$' && 
      sec[i+1] == '|' &&                         
      sec[i+2] == '$')   
  { 
    BUF_DUMP(); SET_STYLE(STYLE_TAG, COL_TAG);  
    (*syntax) = false; skip_char = true; i += 2; 
  }
  if (i+2 < sec_len    &&
      sec[i]   == '$' && 
      sec[i+1] == '?' &&                         
      sec[i+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_LINK, COL_LINK);  
    (*syntax) = false; skip_char = true; i += 2; 
  }

  // -- syntax highlighting --
  if (i+4 < sec_len    &&
       sec[i]   == '$' && 
      (sec[i+1] == 's' || sec[i+1] == 'S') &&                         
      (sec[i+2] == 'y' || sec[i+2] == 'Y') &&                         
      (sec[i+3] == 'n' || sec[i+3] == 'N') &&                         
       sec[i+4] == '$')    
  { BUF_DUMP(); (*syntax) = !(*syntax); skip_char = true; i += 4; }  // $$ resets it
  
  // icons -> $icon::bash$
  if (i+5 < sec_len   &&
       sec[i]   == '$' && 
       (sec[i+1] == 'i' || sec[i+1] == 'I') && 
       (sec[i+2] == 'c' || sec[i+2] == 'C') && 
       (sec[i+3] == 'o' || sec[i+3] == 'O') && 
       (sec[i+4] == 'n' || sec[i+4] == 'N') && 
       sec[i+5] == ':' )
  { 
    BUF_DUMP(); 
    // SET_STYLE(PF_UNDERLINE, cur_pf_color);  
    
    i += 6; // skip $icon: 
    if (core_data->use_utf8 && core_data->use_icons)
    {
      // print icon 
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != ':')
      { buf[buf_pos++] = sec[i++]; }
      BUF_DUMP(); 
      // skip :text$
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$')
      { i++; }
    }
    else // no icons
    {
      // skip icon f.e. :
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != ':')
      { i++; }
      i++; // skip the ':'
      // print text after :
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$')
      { buf[buf_pos++] = sec[i++]; }
      BUF_DUMP(); 
    }
  }

  // file -> $file:path/to/file.txt$
  if (i+5 < sec_len   &&
       sec[i]   == '$' && 
       (sec[i+1] == 'f' || sec[i+1] == 'F') && 
       (sec[i+2] == 'i' || sec[i+2] == 'I') && 
       (sec[i+3] == 'l' || sec[i+3] == 'L') && 
       (sec[i+4] == 'e' || sec[i+4] == 'E') && 
       sec[i+5] == ':' )
  { 
    BUF_DUMP(); 
    // SET_STYLE(PF_UNDERLINE, cur_pf_color);  
    
    i += 6; // skip $file: 
    
    // get path 
    char path[256];
    int  path_len = 0;
    STRCPY(path, cur_file_path);
    path_len = strlen(path);
    while (path_len > 0 && path[path_len] != '\\' && path[path_len] != '/') { path_len--; }
    path_len++;
    char* name = path + path_len; // points to path given in sheet inside 'path'
    while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$' && sec[i] != ':')
    // { buf[buf_pos++] = sec[i++]; }
    { path[path_len++] = sec[i++]; }
    path[path_len] = '\0';
    int name_len = strlen(name);

    int  box_width  = core_data->text_box_width;
    int  box_height = core_data->text_box_height;
    #define BOX_INDENT_MAX 32
    char box_indent[BOX_INDENT_MAX];
    int  box_indent_pos = 0;
    STRCPY(box_indent, core_data->text_box_indent);
    // indent, width, height, defined
    if (sec[i] == ':')
    {
      char tmp[64];
      int  tmp_pos = 0;
      i++; // skip ':'
      // read indent
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$' && sec[i] != ':')
      { tmp[tmp_pos++] = sec[i++]; }
      tmp[tmp_pos] = '\0';
      int indent_count = atoi(tmp);
      if (indent_count > BOX_INDENT_MAX) 
      { P_ERR("$file:<path>:<indent>:<width>:<height>$, max indent is: %d\n", BOX_INDENT_MAX); indent_count = BOX_INDENT_MAX -1; }
      for (; indent_count > 0; --indent_count) { box_indent[box_indent_pos++] = ' '; }
      box_indent[box_indent_pos] = '\0';
      
      // read width 
      tmp_pos = 0;
      i++; // skip ':'
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$' && sec[i] != ':')
      { tmp[tmp_pos++] = sec[i++]; }
      tmp[tmp_pos] = '\0';
      box_width = atoi(tmp);
      
      // read height 
      tmp_pos = 0;
      i++; // skip ':'
      while (i < sec_len && sec[i] != '\n' && sec[i] != '\0' && sec[i] != '$' && sec[i] != ':')
      { tmp[tmp_pos++] = sec[i++]; }
      tmp[tmp_pos] = '\0';
      box_height = atoi(tmp);
    }

    // load file
    char* file;
    int file_len;
    {
      FILE* f;
      f = fopen(path, "rb");
      if (f == NULL) { goto ERROR_LABEL; }
      // get len of file
      fseek(f, 0, SEEK_END);
      file_len = ftell(f);
      if (file_len <= 0) { goto ERROR_LABEL; }
      fseek(f, 0, SEEK_SET);
      file_len++;   // for null-terminator
      // alloc memory 
      file = calloc(1, file_len);
      if (file == NULL) { goto ERROR_LABEL; }
      // fill text buffer
      fread(file, sizeof(char), file_len, f);
      if (strlen(file) <= 0) { FREE(file); goto ERROR_LABEL; }
      fclose(f);
      file[file_len -1] = '\0';
    }

    // print file
    {
      // PF("%s", file);
      #define SPACES_10 "          " 
      #define SPACES_50 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10
      char spaces[]  = SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50 SPACES_50;
      u32 spaces_len = sizeof(spaces) / sizeof(spaces[0]);
      ERR_CHECK(box_width < spaces_len, 
                "box_width: %d is max rn bc. spaces string\n", spaces_len);
      int border_width = box_width -1 +2 +1;
      int file_lines = 0;
      int lines = 0;

      PF("\r%s╭", box_indent); 
      for (int w = name_len +3; w > 0; --w) { PF("─"); } 
      PF("╮\n");
      PF("%s│ %s: │\n", box_indent, name);
      PF("%s├", box_indent);
      for (int w = 0; w < border_width; ++w) 
      { PF(w == name_len+3 ? "┴" : "─"); }
      if (border_width < name_len+3)
      {
        PF("┬");
        for (int w = (name_len +3) - border_width -1; w > 0; --w) { PF("─"); } 
        PF("╯\n");
      }
      else { PF("╮\n"); }

      int width = 0;
      int last_pos = 0;
      for (int t = 0; t < file_len; ++t)
      {
        if (file[t] == '\n')
        {
          lines++;
          if (lines >= box_height)  // check if max lines exceeded
          {
            PF("%s│ ...", box_indent); 
            PF("%.*s │", box_width +1 -4, spaces);
            PF("\n");
            break;
          }
          else
          {
            bool includes_newline = width <= box_width;
            file_lines += includes_newline;
            int real_width = width > box_width ? box_width : width;
            width = width < box_width ? width : box_width;
            if (includes_newline) { width -= 2; }

            // PF("width: %d, cd->w-w: %d\n", width, box_width -width);
            PF("%s│ %.*s", box_indent, width, file +last_pos); 
            PF("%.*s │", box_width -width, spaces);
            PF("\n");
            // if (includes_newline) { width += 2; }
            
            // last_pos += real_width +1;
            last_pos += real_width;
            if (includes_newline) { last_pos = t +1; }
            else 
            { 
              // last_pos -= 1; 
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
      PF("%s╰", box_indent);
      for (int w = border_width; w > 0; --w) { PF("─"); }
      PF("╯\n");
      P_V(lines);
    }
    
    FREE(file);
    goto  END_LABEL;

  ERROR_LABEL:;
    PF_COLOR(PF_RED);
    PF("[ERROR] coudnt find $file:%s$\n", name);
    PF_COLOR(cur_pf_color);

  END_LABEL:;
    // BUF_DUMP(); 
    skip_char = true; 
  }

  #undef buf_pos
  #undef i     
  #undef skip_char
}

#undef BUF_DUMP
#undef BUF_DUMP_OFFSET
#undef DUMP_COLORED
#undef DUMP_TYPE
#undef SET_STYLE
#undef SET_COLOR
#undef SET_DEFAULT_STYLE
#undef RESET_DEFAULT_STYLE

// @TODO: make keword an array and check all keywords are in section
void doc_count_lines_dir(const char* dir_path, int* lines)
{
  char path[256];
  struct dirent* dp;
  DIR* dir = opendir(dir_path);
  // unable to open directory stream
  if (!dir) 
  {
    return; 
  }
  // P_STR(dir_path);
  

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
        // add slash if missing
        char* slash = dir_path[strlen(dir_path) -1] == '/'  ? "" :
                      dir_path[strlen(dir_path) -1] == '\\' ? "" : "\\";
        SPRINTF(300, buf, "%s%s%s", dir_path, slash, dp->d_name);
        // P_STR(buf);
        doc_count_lines_file(buf, lines);
      }
      

      // construct new path from our base path
      strcpy(path, dir_path);
      path[strlen(path) -1] = '\0';
      strcat(path, "\\");
      strcat(path, dp->d_name);
      
      doc_count_lines_dir(path, lines); // search recursively
    }
  }

  // close the stream
  closedir(dir);
}
void doc_count_lines_file(const char* path, int* lines)
{
  if (!check_file_exists(path)) { return; }
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);
  
  int lines_count = 0;

  for (int i = 0; i < txt_len && txt[i] != '\0'; ++i)
  {
    if (txt[i] == '\n') { lines_count++; } 
  }

  (*lines) += lines_count;
  // PF("\"%s\": %d - %d\n", path, lines_count, (*lines));
  FREE(txt);
}


