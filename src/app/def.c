#include "app/def.h"
#include "app/file_io.h"
#include "app/style.h"
#include "app/core_data.h"

#include "stb/stb_ds.h" // STB_DS_IMPLEMEMTATION in main.c

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>



void def_print_result(search_result_t* r)
{
  core_data_t* core_data = core_data_get();

  // @NOTE: no syntaxt highlighting
  if (!core_data->style_act)
  {
    PF("%s\n", r->txt);
    return;
  }

  // @NOTE: syntax highlighted 

  // PF_COLOR(PF_PURPLE); PF("\n%s", r->txt); PF_COLOR(PF_WHITE); 
  int  len = strlen(r->txt);
  char buf[512];
  int  buf_pos = 0;
  
  #define BUF_DUMP()        \
      buf[buf_pos] = '\0';  \
      PF("%s", buf);        \
      buf_pos = 0;

  #define DUMP_COLORED(n, col)                                  \
      { BUF_DUMP();                                             \
      PF_COLOR(col);                                            \
      int j = 0;                                                \
      while (j < (n)) { buf[buf_pos++] = r->txt[i++]; j++; }    \
      BUF_DUMP();                                               \
      PF_COLOR(PF_WHITE); }
  

  for (int i = 0; i < len -1; ++i)
  {

    // highlight all c syntax
    // style_highlight_c(r->txt, buf, &buf_pos, (int*)&i);
    style_highlight_c(r->txt, buf, &buf_pos, &i);
    
    // -- copy from sec --
    buf[buf_pos++] = r->txt[i];
   
    
    // @NOTE: old system, all generic c syntax now gets highlighted in style.c

    // u32 j = 0;
    // while(isalnum(r->txt[i +j]) || r->txt[i +j] == '_') 
    // { j++; }

    // // @NOTE: highlight functions
    // if (r->txt[i +j] == '(' ) // && isalpha(r->txt[i +j]))
    // {      
    //   BUF_DUMP();
    //   PF_COLOR(COL_FUNC);
    //   while (j > 0) { buf[buf_pos++] = r->txt[i++]; j--; }
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }

    // // // @NOTE: highlight comments
    // if (r->txt[i +0] == '/' && r->txt[i +1] == '/')
    // {
    //   BUF_DUMP();
    //   PF_STYLE(PF_ITALIC);
    //   PF_STYLE_COL(PF_DIM, COL_COMMENT);
    //   while (r->txt[i] != '\n' && r->txt[i] != '\0' && i < len) 
    //   { buf[buf_pos++] = r->txt[i++]; }
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    //   PF_STYLE(PF_NORMAL);
    // }    
  
    // 
    // // -- types --
    // if (!isalnum(r->txt[i -1]) && r->txt[i -1] != '|')
    // {
    //   if (r->txt[i +0] == 'v' && r->txt[i +1] == 'o' &&
    //       r->txt[i +2] == 'i' && r->txt[i +3] == 'd')
    //   { DUMP_COLORED(4, COL_TYPE); }
    //   if (r->txt[i +0] == 'i' && r->txt[i +1] == 'n' &&
    //       r->txt[i +2] == 't')
    //   { DUMP_COLORED(3, COL_TYPE); }
    //   if (r->txt[i +0] == 'f' && r->txt[i +1] == 'l' &&
    //       r->txt[i +2] == 'o' && r->txt[i +3] == 'a' &&
    //       r->txt[i +4] == 't')
    //   { DUMP_COLORED(5, COL_TYPE); }
    //   if (r->txt[i +0] == 'd' && r->txt[i +1] == 'o' &&
    //       r->txt[i +2] == 'u' && r->txt[i +3] == 'b' &&
    //       r->txt[i +4] == 'l' && r->txt[i +5] == 'e')
    //   { DUMP_COLORED(6, COL_TYPE); }
    //   if (r->txt[i +0] == 's' && r->txt[i +1] == 'h' &&
    //       r->txt[i +2] == 'o' && r->txt[i +3] == 'r' &&
    //       r->txt[i +4] == 't')
    //   { DUMP_COLORED(5, COL_TYPE); }
    //   if (r->txt[i +0] == 'l' && r->txt[i +1] == 'o' &&
    //       r->txt[i +2] == 'n' && r->txt[i +3] == 'g')
    //   { DUMP_COLORED(4, COL_TYPE); }
    //   if (r->txt[i +0] == 's' && r->txt[i +1] == 'i' &&
    //       r->txt[i +2] == 'z' && r->txt[i +3] == 'e' &&
    //       r->txt[i +4] == '_' && r->txt[i +5] == 't')
    //   { DUMP_COLORED(6, COL_TYPE); }
    //   if (r->txt[i +0] == 'u' && r->txt[i +1] == 'n' &&
    //       r->txt[i +2] == 's' && r->txt[i +3] == 'i' &&
    //       r->txt[i +4] == 'g' && r->txt[i +5] == 'n' &&
    //       r->txt[i +6] == 'e' && r->txt[i +7] == 'd')
    //   { DUMP_COLORED(8, COL_TYPE); }
    //   if (r->txt[i +0] == 'e' && r->txt[i +1] == 'n' &&
    //       r->txt[i +2] == 'u' && r->txt[i +3] == 'm')
    //   { DUMP_COLORED(4, COL_TYPE); }
    //   if (r->txt[i +0] == 's' && r->txt[i +1] == 't' &&
    //       r->txt[i +2] == 'r' && r->txt[i +3] == 'u' &&
    //       r->txt[i +4] == 'c' && r->txt[i +5] == 't')
    //   { DUMP_COLORED(6, COL_TYPE); }
    //   if (r->txt[i +0] == 'c' && r->txt[i +1] == 'h' &&
    //       r->txt[i +2] == 'a' && r->txt[i +3] == 'r')
    //   { DUMP_COLORED(4, COL_TYPE); }
    //   if (r->txt[i +0] == 'b' && r->txt[i +1] == 'o' &&
    //       r->txt[i +2] == 'o' && r->txt[i +3] == 'l')
    //   { DUMP_COLORED(4, COL_TYPE); }
    //   if (r->txt[i +0] == 'c' && r->txt[i +1] == 'o' &&
    //       r->txt[i +2] == 'n' && r->txt[i +3] == 's' &&
    //       r->txt[i +4] == 't')
    //   { DUMP_COLORED(5, COL_TYPE); }
    // }
    // // -- values --

    // if (isdigit(r->txt[i]))                      // numbers
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   int j = 0;
    //   while (isdigit(r->txt[i +j]) || r->txt[i +j] == '.' || r->txt[i +j] == 'f') { j++; }
    //   while (j > 0) { buf[buf_pos++] = r->txt[i++]; j--; }
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }
    // if (r->txt[i +0] == 't' && r->txt[i +1] == 'r' &&
    //     r->txt[i +2] == 'u' && r->txt[i +3] == 'e')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (r->txt[i +0] == 'f' && r->txt[i +1] == 'a' &&
    //     r->txt[i +2] == 'l' && r->txt[i +3] == 's' &&
    //     r->txt[i +4] == 'e')
    // { DUMP_COLORED(5, COL_VALUE); }
    // if (r->txt[i +0] == 'N' && r->txt[i +1] == 'U' &&
    //     r->txt[i +2] == 'L' && r->txt[i +3] == 'L')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (r->txt[i] == '"')                        // strings
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   buf[buf_pos++] = r->txt[i++];
    //   while (r->txt[i] != '"') { buf[buf_pos++] = r->txt[i++]; }
    //   buf[buf_pos++] = r->txt[i++];
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }
    // if (r->txt[i] == '\'' && r->txt[i +2] == '\'')  // chars
    // {
    //   BUF_DUMP();
    //   PF_COLOR(COL_VALUE);
    //   buf[buf_pos++] = r->txt[i++];
    //   buf[buf_pos++] = r->txt[i++];
    //   buf[buf_pos++] = r->txt[i++];
    //   BUF_DUMP();
    //   PF_COLOR(PF_WHITE);
    // }



    // // -- copy from txt --
    // buf[buf_pos++] = r->txt[i];
    
  }

  PF_COLOR(PF_WHITE);
  PF_STYLE(PF_NORMAL);
  BUF_DUMP();
  

  PF("\n -> '%s', line: [%d]\n", r->file, r->lne);
}


#define MAX_DIR_DEPTH 2
void def_search_dir(const char* dir_path, const char* keyword, int* n, search_result_t** search_results, int* dir_depth)
{  
  char path[256];
  struct dirent* dp;
  DIR* dir = opendir(dir_path);
  // unable to open directory stream
  if (!dir) { /*P_ERR("directory not found"); PF("dir: %s\n", dir_path);*/ return; }
  // if (*dir_depth >= MAX_DIR_DEPTH) { P_INT(*dir_depth); return; }
  P_INT(*dir_depth);
  (*dir_depth)++;

  // recursively read the directory and its sub-directories
  while ((dp = readdir(dir)) != NULL)
  {
    // check that the file currently read isn't a directory
    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
    {
      // check_file(dp->d_name, strlen(dp->d_name), dir_path);
      if (dp->d_name[dp->d_namlen -2] == '.' &&
          dp->d_name[dp->d_namlen -1] == 'h' )
      {
        // P_STR(dp->d_name);
        // P_STR(dir_path);
        char buf[300];
        SPRINTF(300, buf, "%s/%s", dir_path, dp->d_name);
        if (def_search_section(buf, dp->d_name, keyword, search_results)) 
        { 
          *n += 1; 
          // P("n+++");
        }
        // P("-----------------------");
      }

      // construct new path from our base path
      strcpy(path, dir_path);
      strcat(path, "/");
      // strcat(path, "\\");
      strcat(path, dp->d_name);

      def_search_dir(path, keyword, n, search_results, dir_depth); // search recursively
    }
  }

  // close the stream
  closedir(dir);
}

bool def_search_section(const char* path, const char* file, const char* keyword, search_result_t** search_results)
{
  if (!check_file_exists(path)) { PF("[!] file '%s': '%s' no existo !!! \n", path, file); return false; }
  int   txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);

  search_result_t result = { .start = 0, .end = 0, .lne = 0, .file = "unknown" };
  // @TODO: see if better way to do this
  bool found_one = false;
  bool found = false;
  u32 lne_counter = 1;  // starts on line 1 not 0
  int start, end = 0;
  for (start = 1; start < txt_len; start++) // txt[start] != '\0'
  {
    if (txt[start] == '\n') { lne_counter++; }  // count lines

    // @TODO: struct defninitions
    // @NOTE: func definitions
    if (txt[start] == keyword[0] && 
        isspace(txt[start -1]))
    {
      found_one = true;
      found = true;
      int j = 0;  // 
      for (j = 0; keyword[j] != '\0'; j++)
      {
        if (txt[start +j] != keyword[j])
        { found = false; break; }
      }
      if (txt[start +j] != '(') 
      { found = false; }

      // @NOTE: found is in comment
      for (u32 k = 0; start - k >= 0; ++k) 
      { 
        if (txt[start -k] == '\n') { break;} 
        if (txt[start -k] == '/' && txt[start -k -1] == '/') { found = false; break;}  
      }

      end = start + j;
      
      if (found)
      {
        // @NOTE: include comments
        bool comment_in_line = true;
        for (u32 k = start; k >= 0; --k)
        {
          // if ((txt[k -1] == '\n' || k -1 == 0) &&
          if (txt[k +0] == '/' && txt[k +1] == '/')
          { start = k; comment_in_line = true; }

          if ((txt[k] == '\n' || k == 0) && !comment_in_line) 
          { break; }

          if (txt[k] == '\n')
          { comment_in_line = false; }       

        }
        // @NOTE: go to start of line
        for (u32 k = start; k > 0; --k)
        {
          if (txt[k] == '\n' || k == 0) { start = k +1; break; }
        }

        // @NOTE: include args
        for (;end < txt_len; ++end)
        {
          if (txt[end] == ';')  { end++; break; }
          if (txt[end] == '\n') { break; }
        }
      }

    }
   
    if (found) 
    { 
      char tmp = txt[end];
      txt[end] = '\0'; 

      // PF("found: '%s' in '%s' on line: %d looking for '%s'\n", (txt + start), file, lne_counter, keyword); 
      
      result.start = start;
      result.end   = end;
      result.lne   = lne_counter;
      strcpy(result.txt, txt + start);
     
      // append dir to file name, e.g. 'dir/file.h'
      u32 slash_count = 0;
      u32 i = 0;
      for (i = strlen(path); i >= 0; --i) 
      {  
        if (path[i] == '/' || path[i] == '\\') { slash_count++; }
        if (slash_count >= 2) { i++; break; }
      }
      strcpy(result.file, path +i);
      
      arrput(*search_results, result);

      txt[end] = tmp;
      start = end;
      found = false;
    }

  }
  FREE(txt);
  
  return found_one; // found_one;
}


