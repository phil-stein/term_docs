#include "app/doc.h"
#include "app/file_io.h"
#include "app/style.h"
#include "app/core_data.h"
#include "global/global.h"

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>

u32 cur_pf_color = PF_WHITE; 
u32 cur_pf_style = PF_NORMAL;

u32 default_pf_color = PF_WHITE; 
u32 default_pf_style = PF_NORMAL;

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
      // print location
      if (core_data->print_loc_act)
      {
        int lines_count = 1;  // idk has to be 1
        for (int i = 0; i < txt_len && i <= start; ++i)
        { if (txt[i] == '\n') { lines_count++;} }
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
      doc_print_section(txt + start, end - start, keywords[0], file);
      sections_found_count++;
    }

    // skip to next section
    start = end;
  }

  FREE(txt);

  return sections_found_count > 0;
}


void doc_print_section(char* sec, int sec_len, const char* keyword, const char* file)
{
  core_data_t* core_data = core_data_get();
  
  DOC_PF_COLOR(PF_PURPLE);
  P_LINE_STR("%s|%s ", file, keyword); 
  DOC_PF_COLOR(PF_WHITE);
  
  doc_color_code_section(sec, sec_len);
   
  DOC_PF_COLOR(PF_PURPLE);
  P_LINE(); 
  DOC_PF_COLOR(PF_WHITE);
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
    SET_COLOR(col);                                        \
    int j = 0;                                            \
    while (j < (n)) { buf[buf_pos++] = sec[i++]; j++; }   \
    BUF_DUMP();                                           \
    SET_COLOR(PF_WHITE); }

#define SET_STYLE(m, c) if (core_data->style_act) { PF_STYLE((m), (c)); cur_pf_style = (m); cur_pf_color = (c); }
#define SET_COLOR(c)    if (core_data->style_act) { PF_COLOR((c)); cur_pf_color = (c); }

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
      buf[buf_pos++] = ERROR_ICON;  
      buf[buf_pos++] = ERROR_ICON +1;  
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
      SET_COLOR(PF_WHITE);
    }
    else if (sec[i] == '!' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- info --
    if (sec[i] == '~' && sec[i -1] != '\\')
    {
      BUF_DUMP();
      SET_STYLE(PF_ITALIC, COL_INFO);
      SET_DEFAULT_STYLE(PF_ITALIC, COL_INFO);
      buf[buf_pos++] = sec[i++];
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
      SET_COLOR(PF_WHITE);
    }
    else if (sec[i] == '~' && sec[i -1] == '\\') { buf_pos -= 1; BUF_DUMP(); } // skip '\'

    // -- links --
    if (sec[i] == '?' && sec[i -1] != '\\')
    {
      BUF_DUMP();
      buf[buf_pos++] = sec[i++];
      while ((sec[i] != '\n' && sec[i] != '\0') && 
             (sec[i] != '?' || sec[i -1] == '\\'))
      {
        // escaped ?
        if (sec[i] == '\\' && sec[i +1] == '?') { i++; continue; }
        buf[buf_pos++] = sec[i++]; 
      }
      if (sec[i] == '?') { i++; }
      SET_STYLE(PF_DIM, COL_LINK);
      SET_STYLE(PF_ITALIC, COL_LINK);
      BUF_DUMP();
      SET_COLOR(PF_WHITE);
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
      SET_COLOR(PF_WHITE);
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
  { BUF_DUMP(); SET_STYLE(default_pf_style, default_pf_color); (*syntax) = true; skip_char = true; i++; }
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


