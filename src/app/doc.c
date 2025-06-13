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

  // core_data_t* core_data = core_data_get();
  
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
            // // DOC_PF("in loop: "); P_BOOL(keyword_found);
            // for (int c = 0; c  < strlen(keyword); ++c)
            // { DOC_PF("%c", txt[section +c]); }
            // DOC_PF("\n");
  
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
        DOC_PF("location");
        DOC_PF_COLOR(PF_WHITE);
        DOC_PF(":\n -> ");
        DOC_PF_COLOR(PF_CYAN);
        DOC_PF("file"); 
        DOC_PF_COLOR(PF_WHITE);
        DOC_PF(": %s\n -> ", path);
        DOC_PF_COLOR(PF_CYAN);
        DOC_PF("line");
        DOC_PF_COLOR(PF_WHITE);
        DOC_PF(": %d\n", lines_count);
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
  
  char _keywords[128] = "";
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
    DOC_PF("%s", buf);        \
    buf_pos = 0;

#define BUF_DUMP_OFFSET(offset) \
    buf[buf_pos] = '\0';        \
    DOC_PF("%s", buf +(offset));    \
    buf_pos = 0;

#define DUMP_COLORED(n, col)                              \
    { BUF_DUMP();                                         \
    SET_COLOR(col);                                       \
    int j = 0;                                            \
    while (j < (n)) { buf[buf_pos++] = sec[i++]; j++; }   \
    BUF_DUMP();                                           \
    SET_COLOR(PF_WHITE);                                  \
    DOC_PF_STYLE_RESET();                                 \
    }

#define SET_STYLE(m, c) if (core_data->style_act) { PF_STYLE((m), (c)); cur_pf_style = (m); cur_pf_color = (c); }
#define SET_COLOR(c)    if (core_data->style_act) { DOC_PF_COLOR((c)); cur_pf_color = (c); }

#define RESET_STYLE() if (core_data->style_act) { PF_STYLE_RESET(); RESET_DEFAULT_STYLE(); cur_pf_style = PF_NORMAL; cur_pf_color = PF_WHITE; }

#define SET_DEFAULT_STYLE(m, c)   if (core_data->style_act) { default_pf_style = (m); default_pf_color = (c); }
#define RESET_DEFAULT_STYLE()     if (core_data->style_act) { default_pf_style = PF_NORMAL; default_pf_color = PF_WHITE; }

void doc_color_code_section(char* sec, int len)
{
  // core_data_t* core_data = core_data_get();
  // @NOTE: no syntax highlighting
  // if (!core_data->style_act)
  // {
  //   // DOC_PF("%s\n", sec);
  //   u32 i = 0;
  //   while (sec[i] != '\0')
  //   {
  //     if (sec[i] == '\r') { DOC_PF("CR"); }
  //     if (sec[i] == '\n') { DOC_PF("*"); }
  //     DOC_PF("%c", sec[i++]);
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
      if (style_highlight_lang(sec, buf, &buf_pos, &i))
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
  DOC_PF("%s", buf);
  DOC_PF("\n");
  DOC_PF("\n");   // twice bc. cutting off in for (len -1)
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
  #define sec_pos   (*i_ptr)
  #define skip_char (*skip_char_ptr)
  
  
  // core_data_t* core_data = core_data_get();

  if (sec[sec_pos] == '$' && sec[sec_pos+1] == '$')                             // reset to normal
  { 
    BUF_DUMP(); RESET_STYLE();  SET_STYLE(default_pf_style, default_pf_color); 
    (*syntax) = true; skip_char = true; sec_pos++; 
  }
  // white
  if (sec_pos+6 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'w' || sec[sec_pos+1] == 'W') && 
      (sec[sec_pos+2] == 'h' || sec[sec_pos+2] == 'H') && 
      (sec[sec_pos+3] == 'i' || sec[sec_pos+3] == 'I') && 
      (sec[sec_pos+4] == 't' || sec[sec_pos+4] == 'T') && 
      (sec[sec_pos+5] == 'e' || sec[sec_pos+5] == 'E') && 
       sec[sec_pos+6] == '$' )   // set color to white
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_WHITE); skip_char = true; sec_pos += 6; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'w' || sec[sec_pos+1] == 'W') && 
       sec[sec_pos+2] == '$' )   // set color to white
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_WHITE); skip_char = true; sec_pos += 2; }
  // red
  if (sec_pos+4 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'r' || sec[sec_pos+1] == 'R') &&                         
      (sec[sec_pos+2] == 'e' || sec[sec_pos+2] == 'E') && 
      (sec[sec_pos+3] == 'd' || sec[sec_pos+3] == 'D') && 
       sec[sec_pos+4] == '$')    // set color to red
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_RED); skip_char = true; sec_pos += 4; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'r' || sec[sec_pos+1] == 'R') &&                         
       sec[sec_pos+2] == '$')    // set color to red
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_RED); skip_char = true; sec_pos += 2; }
  // black 
  if (sec_pos+6 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'b' || sec[sec_pos+1] == 'B') && 
      (sec[sec_pos+2] == 'l' || sec[sec_pos+2] == 'L') && 
      (sec[sec_pos+3] == 'a' || sec[sec_pos+3] == 'A') && 
      (sec[sec_pos+4] == 'c' || sec[sec_pos+4] == 'C') && 
      (sec[sec_pos+5] == 'k' || sec[sec_pos+5] == 'K') && 
       sec[sec_pos+6] == '$' )   // set color to black
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLACK); skip_char = true; sec_pos += 6; }
  // green
  if (sec_pos+6 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'g' || sec[sec_pos+1] == 'G') && 
      (sec[sec_pos+2] == 'r' || sec[sec_pos+2] == 'R') && 
      (sec[sec_pos+3] == 'e' || sec[sec_pos+3] == 'E') && 
      (sec[sec_pos+4] == 'e' || sec[sec_pos+4] == 'E') && 
      (sec[sec_pos+5] == 'n' || sec[sec_pos+5] == 'N') && 
       sec[sec_pos+6] == '$' )   // set color to green
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_GREEN); skip_char = true; sec_pos += 6; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'g' || sec[sec_pos+1] == 'G') && 
       sec[sec_pos+2] == '$' )   // set color to green
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_GREEN); skip_char = true; sec_pos += 2; }
  // yellow
  if (sec_pos+7 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'y' || sec[sec_pos+1] == 'Y') && 
      (sec[sec_pos+2] == 'e' || sec[sec_pos+2] == 'E') && 
      (sec[sec_pos+3] == 'l' || sec[sec_pos+3] == 'L') && 
      (sec[sec_pos+4] == 'l' || sec[sec_pos+4] == 'L') && 
      (sec[sec_pos+5] == 'o' || sec[sec_pos+5] == 'O') && 
      (sec[sec_pos+6] == 'w' || sec[sec_pos+6] == 'W') && 
       sec[sec_pos+7] == '$' )   // set color to yellow
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_YELLOW); skip_char = true; sec_pos += 7; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'y' || sec[sec_pos+1] == 'Y') && 
       sec[sec_pos+2] == '$' )   // set color to yellow
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_YELLOW); skip_char = true; sec_pos += 2; }
  // blue
  if (sec_pos+5 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'b' || sec[sec_pos+1] == 'B') && 
      (sec[sec_pos+2] == 'l' || sec[sec_pos+2] == 'L') && 
      (sec[sec_pos+3] == 'u' || sec[sec_pos+3] == 'U') && 
      (sec[sec_pos+4] == 'e' || sec[sec_pos+4] == 'E') && 
       sec[sec_pos+5] == '$' )   // set color to blue
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLUE); skip_char = true; sec_pos += 5; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'b' || sec[sec_pos+1] == 'B') && 
       sec[sec_pos+2] == '$' )   // set color to blue
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_BLUE); skip_char = true; sec_pos += 2; }
  // purple
  if (sec_pos+7 < sec_len   &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'p' || sec[sec_pos+1] == 'P') && 
      (sec[sec_pos+2] == 'u' || sec[sec_pos+2] == 'U') && 
      (sec[sec_pos+3] == 'r' || sec[sec_pos+3] == 'R') && 
      (sec[sec_pos+4] == 'p' || sec[sec_pos+4] == 'P') && 
      (sec[sec_pos+5] == 'l' || sec[sec_pos+5] == 'L') && 
      (sec[sec_pos+6] == 'e' || sec[sec_pos+6] == 'E') && 
       sec[sec_pos+7] == '$' )   // set color to purple
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_PURPLE); skip_char = true; sec_pos += 7; }
  if (sec_pos+2 < sec_len   &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'p' || sec[sec_pos+1] == 'P') && 
       sec[sec_pos+2] == '$' )   // set color to purple
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_PURPLE); skip_char = true; sec_pos += 2; }
  // cyan
  if (sec_pos+5 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'c' || sec[sec_pos+1] == 'C') && 
      (sec[sec_pos+2] == 'y' || sec[sec_pos+2] == 'Y') && 
      (sec[sec_pos+3] == 'a' || sec[sec_pos+3] == 'A') && 
      (sec[sec_pos+4] == 'n' || sec[sec_pos+4] == 'N') && 
       sec[sec_pos+5] == '$' )   // set color to cyan
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_CYAN); skip_char = true; sec_pos += 5; }
  if (sec_pos+2 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'c' || sec[sec_pos+1] == 'C') && 
       sec[sec_pos+2] == '$' )   // set color to cyan
  { BUF_DUMP(); SET_STYLE(cur_pf_style, PF_CYAN); skip_char = true; sec_pos += 2; }

  // -- modes -- 

  // italic
  if (sec_pos+7 < sec_len   &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'i' || sec[sec_pos+1] == 'I') && 
      (sec[sec_pos+2] == 't' || sec[sec_pos+2] == 'T') && 
      (sec[sec_pos+3] == 'a' || sec[sec_pos+3] == 'A') && 
      (sec[sec_pos+4] == 'l' || sec[sec_pos+4] == 'L') && 
      (sec[sec_pos+5] == 'i' || sec[sec_pos+5] == 'I') && 
      (sec[sec_pos+6] == 'c' || sec[sec_pos+6] == 'C') && 
       sec[sec_pos+7] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_ITALIC, cur_pf_color); skip_char = true; sec_pos += 7; }
  if (sec_pos+2 < sec_len   &&
       sec[sec_pos]   == '$' && 
       sec[sec_pos+1] == '/' && 
       sec[sec_pos+2] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_ITALIC, cur_pf_color); skip_char = true; sec_pos += 2; }
  // dim
  if (sec_pos+4 < sec_len   &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 'd' || sec[sec_pos+1] == 'D') && 
      (sec[sec_pos+2] == 'i' || sec[sec_pos+2] == 'I') && 
      (sec[sec_pos+3] == 'm' || sec[sec_pos+3] == 'M') && 
       sec[sec_pos+4] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_DIM, cur_pf_color); skip_char = true; sec_pos += 4; }
  // underline
  if (sec_pos+10 < sec_len   &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+ 1] == 'u' || sec[sec_pos+1] == 'U') && 
      (sec[sec_pos+ 2] == 'n' || sec[sec_pos+2] == 'N') && 
      (sec[sec_pos+ 3] == 'd' || sec[sec_pos+3] == 'D') && 
      (sec[sec_pos+ 4] == 'e' || sec[sec_pos+4] == 'E') && 
      (sec[sec_pos+ 5] == 'r' || sec[sec_pos+5] == 'R') && 
      (sec[sec_pos+ 6] == 'l' || sec[sec_pos+6] == 'L') && 
      (sec[sec_pos+ 7] == 'i' || sec[sec_pos+7] == 'I') && 
      (sec[sec_pos+ 8] == 'n' || sec[sec_pos+8] == 'N') && 
      (sec[sec_pos+ 9] == 'e' || sec[sec_pos+9] == 'E') && 
       sec[sec_pos+10] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_UNDERLINE, cur_pf_color);  skip_char = true; sec_pos += 10; }
  if (sec_pos+2 < sec_len   &&
       sec[sec_pos]   == '$' && 
       sec[sec_pos+1] == '_' && 
       sec[sec_pos+2] == '$' )
  { BUF_DUMP(); SET_STYLE(PF_UNDERLINE, cur_pf_color);  skip_char = true; sec_pos += 2; }


  // -- info / warning / tag / link --
  if (sec_pos+2 < sec_len    &&
      sec[sec_pos]   == '$' && 
      sec[sec_pos+1] == '~' &&                         
      sec[sec_pos+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_INFO, COL_INFO);  
    (*syntax) = false; skip_char = true; sec_pos += 2; 
  }
  if (sec_pos+2 < sec_len    &&
      sec[sec_pos]   == '$' && 
      sec[sec_pos+1] == '!' &&                         
      sec[sec_pos+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_WARNING, COL_WARNING);  
    (*syntax) = false; skip_char = true; sec_pos += 2; 
  }
  if (sec_pos+2 < sec_len    &&
      sec[sec_pos]   == '$' && 
      sec[sec_pos+1] == '|' &&                         
      sec[sec_pos+2] == '$')   
  { 
    BUF_DUMP(); SET_STYLE(STYLE_TAG, COL_TAG);  
    (*syntax) = false; skip_char = true; sec_pos += 2; 
  }
  if (sec_pos+2 < sec_len    &&
      sec[sec_pos]   == '$' && 
      sec[sec_pos+1] == '?' &&                         
      sec[sec_pos+2] == '$')    
  { 
    BUF_DUMP(); SET_STYLE(STYLE_LINK, COL_LINK);  
    (*syntax) = false; skip_char = true; sec_pos += 2; 
  }

  // -- syntax highlighting --
  if (sec_pos+4 < sec_len    &&
       sec[sec_pos]   == '$' && 
      (sec[sec_pos+1] == 's' || sec[sec_pos+1] == 'S') &&                         
      (sec[sec_pos+2] == 'y' || sec[sec_pos+2] == 'Y') &&                         
      (sec[sec_pos+3] == 'n' || sec[sec_pos+3] == 'N') &&                         
       sec[sec_pos+4] == '$')    
  { BUF_DUMP(); (*syntax) = !(*syntax); skip_char = true; sec_pos += 4; }  // $$ resets it
  
  // icons -> $icon::bash$
  if (sec_pos+5 < sec_len   &&
       sec[sec_pos]   == '$' && 
       (sec[sec_pos+1] == 'i' || sec[sec_pos+1] == 'I') && 
       (sec[sec_pos+2] == 'c' || sec[sec_pos+2] == 'C') && 
       (sec[sec_pos+3] == 'o' || sec[sec_pos+3] == 'O') && 
       (sec[sec_pos+4] == 'n' || sec[sec_pos+4] == 'N') && 
       sec[sec_pos+5] == ':' )
  { 
    BUF_DUMP(); 
    // SET_STYLE(PF_UNDERLINE, cur_pf_color);  
    
    sec_pos += 6; // skip $icon: 
    if (core_data->use_utf8 && core_data->use_icons)
    {
      // print icon 
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != ':')
      { buf[buf_pos++] = sec[sec_pos++]; }
      BUF_DUMP(); 
      // skip :text$
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '$')
      { sec_pos++; }
    }
    else // no icons
    {
      // skip icon f.e. :
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != ':')
      { sec_pos++; }
      sec_pos++; // skip the ':'
      // print text after :
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '$')
      { buf[buf_pos++] = sec[sec_pos++]; }
      BUF_DUMP(); 
    }
  }

  // file -> $file:path/to/file.txt$
  if (sec_pos+5 < sec_len   &&
       sec[sec_pos]   == '$' && 
       (sec[sec_pos+1] == 'f' || sec[sec_pos+1] == 'F') && 
       (sec[sec_pos+2] == 'i' || sec[sec_pos+2] == 'I') && 
       (sec[sec_pos+3] == 'l' || sec[sec_pos+3] == 'L') && 
       (sec[sec_pos+4] == 'e' || sec[sec_pos+4] == 'E') && 
       sec[sec_pos+5] == ':' )
  { 
    BUF_DUMP(); 
    // SET_STYLE(PF_UNDERLINE, cur_pf_color);  
    
    sec_pos += 6; // skip $file: 
    
    // get path 
    char path_buf[256];
    int  path_buf_pos = 0;
    char* name; 
    char* path;
    int name_len = 0; 
    STRCPY(path_buf, cur_file_path);
    path_buf_pos = strlen(path_buf);
    while (path_buf_pos > 0 && path_buf[path_buf_pos] != '\\' && path_buf[path_buf_pos] != '/') { path_buf_pos--; }
    path_buf_pos++;
  
    sec_pos++; // skip '"'
    name = path_buf + path_buf_pos; // points to path given in sheet inside 'path'
    while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '"')// && sec[i] != '$' && sec[i] != ':')
    // { buf[buf_pos++] = sec[i++]; }
    { path_buf[path_buf_pos++] = sec[sec_pos] == '\\' ? '/' : sec[sec_pos]; sec_pos++; }
    path_buf[path_buf_pos] = '\0';
    sec_pos++; // skip '"'
    name_len = strlen(name);

    path = path_buf;
    // absolute path
    if (name[0] == '/' || name[0] == '\\' || 
        name[0] == '%' || name[0] == '~' || name[1] == ':')
    {
      path = name;
    }
    // P_STR(name);
    // P_STR(path);

    int  box_width  = core_data->text_box_width;
    int  box_height = core_data->text_box_height;
    #define BOX_INDENT_MAX 32
    char box_indent[BOX_INDENT_MAX];
    int  box_indent_pos = 0;
    STRCPY(box_indent, core_data->text_box_indent);
    // indent, width, height, defined
    if (sec[sec_pos] == ':')
    {
      char tmp[64];
      int  tmp_pos = 0;
      sec_pos++; // skip ':'
      // read indent
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '$' && sec[sec_pos] != ':')
      { tmp[tmp_pos++] = sec[sec_pos++]; }
      tmp[tmp_pos] = '\0';
      int indent_count = atoi(tmp);
      if (indent_count > BOX_INDENT_MAX) 
      { P_ERR("$file:<path>:<indent>:<width>:<height>$, max indent is: %d\n", BOX_INDENT_MAX); indent_count = BOX_INDENT_MAX -1; }
      for (; indent_count > 0; --indent_count) { box_indent[box_indent_pos++] = ' '; }
      box_indent[box_indent_pos] = '\0';
      
      // read width 
      tmp_pos = 0;
      sec_pos++; // skip ':'
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '$' && sec[sec_pos] != ':')
      { tmp[tmp_pos++] = sec[sec_pos++]; }
      tmp[tmp_pos] = '\0';
      box_width = atoi(tmp);
      
      // read height 
      tmp_pos = 0;
      sec_pos++; // skip ':'
      while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '$')
      { tmp[tmp_pos++] = sec[sec_pos++]; }
      tmp[tmp_pos] = '\0';
      box_height = atoi(tmp);
    }
    doc_color_code_color_codes_print_file(box_indent, box_width, box_height, true, name, name_len, path);

    skip_char = true; 
  }

  // doc -> $doc:"malloc"$
  if (sec_pos+5 < sec_len   &&
       sec[sec_pos]   == '$' && 
       (sec[sec_pos+1] == 'd' || sec[sec_pos+1] == 'D') && 
       (sec[sec_pos+2] == 'o' || sec[sec_pos+2] == 'O') && 
       (sec[sec_pos+3] == 'c' || sec[sec_pos+3] == 'C') && 
       sec[sec_pos+4] == ':' )
  { 
    BUF_DUMP(); 
    // SET_STYLE(PF_UNDERLINE, cur_pf_color);  
    
    sec_pos += 5; // skip $doc: 
    
    // get path 
    char cmd_buf[256];
    int  cmd_buf_pos = 0;
    sec_pos++; // skip '"'
    while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '"')
    { cmd_buf[cmd_buf_pos++] = sec[sec_pos] == '\\' ? '/' : sec[sec_pos]; sec_pos++; }
    cmd_buf[cmd_buf_pos] = '\0';
    sec_pos++; // skip '"'
    // P_STR(cmd_buf);
 
    // FILE* doc_file = tmpfile();
    // FILE* doc_file = tmpnam("./doc_cmd.txt");
    FILE* doc_file = fopen("bin/doc_cmd.tmp", "w+");
    if (doc_file == NULL) { P_ERR("failed executing $doc:\"%s\"$ output\n", cmd_buf);  goto ERROR_LABEL;}
    core_data->pf_out = doc_file;
    int found_count = 0;  // counts found matches for keyword
    const char* keywords[] = { (const char*)cmd_buf };
    int keywords_len = 1;
    P_STR(keywords[0]);
    core_data->style_act = false;
    core_data->use_icons = false;
    for (int i = 0; i < core_data->sheet_paths_len; ++i)
    {
      doc_search_dir(core_data->sheet_paths[i], keywords, keywords_len, &found_count);
    }
    if (found_count <= 0) { DOC_PF("not find nothing\n"); goto ERROR_LABEL; }
    core_data->pf_out = stdout;
    core_data->style_act = true;
    core_data->use_icons = true;

    fseek(doc_file, 0, SEEK_END);
    int output_len = ftell(doc_file);
    fseek(doc_file, 0, SEEK_SET);
    output_len++;   // for null-terminator
    // alloc memory 
    char* output = calloc(1, output_len);
    if (output == NULL) { P_ERR("failed reading $doc:\"%s\"$ output\n", cmd_buf);  goto ERROR_LABEL;}
    // fill text buffer
    fread(output, sizeof(char), output_len, doc_file);
    if (strlen(output) <= 0) { FREE(output); P_ERR("failed reading $doc:\"%s\"$ output\n", cmd_buf); goto ERROR_LABEL; }
    fclose(doc_file);
    output[output_len -1] = '\0';

    P_STR(output);
    int  box_width  = core_data->text_box_width;
    int  box_height = core_data->text_box_height;
    #define BOX_INDENT_MAX 32
    char box_indent[BOX_INDENT_MAX];
    int  box_indent_pos = 0;
    STRCPY(box_indent, core_data->text_box_indent);
    style_print_text_box(box_indent, box_width, box_height, false, output, output_len, cmd_buf, cmd_buf_pos);
    // style_print_text_box(box_indent, box_width, box_height, file, file_len, cmd_buf, cmd_buf_pos)
    // doc_color_code_color_codes_print_file(box_indent, box_width, box_height, name, name_len, path);
    
    FREE(output);
    ERROR_LABEL:;
    skip_char = true; 
  }
  if (sec_pos+5 < sec_len   &&
       sec[sec_pos]   == '$' && 
       (sec[sec_pos+1] == 'l' || sec[sec_pos+1] == 'L') && 
       (sec[sec_pos+2] == 'a' || sec[sec_pos+2] == 'A') && 
       (sec[sec_pos+3] == 'n' || sec[sec_pos+3] == 'N') && 
       (sec[sec_pos+4] == 'g' || sec[sec_pos+4] == 'G') && 
       sec[sec_pos+5] == ':' )
  { 
    BUF_DUMP(); 
    
    sec_pos += 6; // skip $lang:

    // get lang name 
    char cmd_buf[256];
    int  cmd_buf_pos = 0;
    sec_pos++; // skip '"'
    while (sec_pos < sec_len && sec[sec_pos] != '\n' && sec[sec_pos] != '\0' && sec[sec_pos] != '"')
    { cmd_buf[cmd_buf_pos++] = sec[sec_pos] == '\\' ? '/' : sec[sec_pos]; sec_pos++; }
    cmd_buf[cmd_buf_pos] = '\0';
    sec_pos++; // skip '"'
    if ( sec[sec_pos] != '$' ) 
    { 
      char c = sec[sec_pos +1];
      sec[sec_pos +1] = '\0';

      PF( "\n"); 
      ERR( "$lang:\"xyz\"$ not terminated with $, but: %c\n -> segment: %s\n -> cmd_buf: %s\n", sec[sec_pos], sec, cmd_buf ); 
      sec[sec_pos +1] = c;
    }
    sec_pos++; // skip '$'
    P_STR(cmd_buf);
    
    if ( cmd_buf_pos >= 1 &&
         ( cmd_buf[0] == 'c' || cmd_buf[0] == 'C' )
       )
    {
      core_data->lang_type = LANG_C;
      P( "LANG_C" );
    }
    else if ( cmd_buf_pos >= 4 &&
              (
                ( cmd_buf[0] == 'o' || cmd_buf[0] == 'O' ) &&
                ( cmd_buf[1] == 'd' || cmd_buf[1] == 'D' ) &&
                ( cmd_buf[2] == 'i' || cmd_buf[2] == 'I' ) &&
                ( cmd_buf[3] == 'n' || cmd_buf[3] == 'N' )
              )
            )
    {
      core_data->lang_type = LANG_ODIN;
      P( "LANG_ODIN" );
    }
    else { P_ERR( "unknown argument in $lang:xyz$ command: '%s'\n", cmd_buf ); }
  }

  #undef buf_pos
  #undef i     
  #undef skip_char
}
void doc_color_code_color_codes_print_file(const char* box_indent, int box_width, int box_height, bool print_line_nr, const char* name, int name_len, const char* path)
{
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

  style_print_text_box(box_indent, box_width, box_height, print_line_nr, file, file_len, name, name_len);
  
  FREE(file);
  goto  END_LABEL;

ERROR_LABEL:;
  DOC_PF_COLOR(PF_RED);
  DOC_PF("[ERROR] coudnt find $file:\"%s\"$\n", name);
  DOC_PF_COLOR(cur_pf_color);

END_LABEL:;
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
  // DOC_PF("\"%s\": %d - %d\n", path, lines_count, (*lines));
  FREE(txt);
}


