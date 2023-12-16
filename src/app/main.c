#include "app/doc.h"
#include "app/def.h"
#include "app/style.h"
#include "app/file_io.h"
#include "app/core_data.h"
#include "app/config.h"

// order is important, io_util & str_util before global
#define IO_UTIL_IMPLEMENTATION       // only define once
#include "global/io_util.h"          // only need to include here, normally included via global.h
#define STR_UTIL_IMPLEMENTATION      // only define once
#include "global/str_util.h"         // only need to include here, normally included via global.h
#include "global/global.h"

#define STB_DS_IMPLEMENTATION   // only define once
#include "stb/stb_ds.h"

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>

typedef enum
{
  SEARCH_DOCUMENTATION  = FLAG(0),
  SEARCH_DEFINITION     = FLAG(1),
  SEARCH_SEARCH         = FLAG(2),
}search_mode_t;


int main(int argc, char** argv)
{
  core_data_t* core_data = core_data_get();

  // -- get executable name --
  // P_STR(_getcwd(NULL, 0));
  GetModuleFileName(NULL, core_data->exec_path, CORE_PATH_MAX);   // get executable location
  assert(core_data->exec_path != NULL);

  // -- get the sheet path either based on executable or macro --
  
  strncpy(core_data->sheets_path, core_data->exec_path, CORE_PATH_MAX);
  assert(core_data->sheets_path != NULL);
  int dirs_walk_back = 1 + DIRS_TO_WALK_BACK_TO_ROOT;
  for (u32 i = strlen(core_data->sheets_path) -1; i > 0; --i)
  {
    if (core_data->sheets_path[i] == '\\') 
    { dirs_walk_back--; if (dirs_walk_back <= 0) { break; } }
    core_data->sheets_path[i] = '\0';
  }
  strcat(core_data->sheets_path, "sheets\\");
  P_STR(core_data->sheets_path);
  
  // -- config file --
  
  #define CONFIG_PATH_MAX 256
  char config_path[CONFIG_PATH_MAX];
  strncpy(config_path, core_data->exec_path, CORE_PATH_MAX);
  assert(config_path != NULL);
  int dirs_walk_back_02 = 1 + DIRS_TO_WALK_BACK_TO_ROOT;
  for (u32 i = strlen(config_path) -1; i > 0; --i)
  {
    if (config_path[i] == '\\') 
    { dirs_walk_back_02--; if (dirs_walk_back_02 <= 0) { break; } }
    config_path[i] = '\0';
  }
  strcat(config_path, "config.doc");
  P_STR(config_path);
  config_read_config_file(config_path);

  // -- arguments --

  // too few arguments, at least 1, i.e. 2 because doc counts
  if (argc < 2)
  { 
    PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
    PF(" provide keyword to search or command, i.e. '"); 
    PF_COLOR(PF_CYAN); PF("-h"); PF_COLOR(PF_WHITE);
    PF("' for help.\n"); 
    exit(1);
  }
 
  // ---- commands ----

  search_mode_t mode = SEARCH_DOCUMENTATION;  // search docs by default
  u32 cmd_count = 0;
  #define WORD_ARR_MAX 24
  char* word_arr[WORD_ARR_MAX] = { NULL }; // all words, aka. not commands, like malloc, git, etc.
  int   word_arr_pos = 0;
  for (u32 i = 1; i < argc; ++i)
  {
    // // @NOTE: '-h' or '-help' for help is in term_docs.sheet
    
    // @NOTE: '-c' or '-color' to disable color and styles
    if ((argv[i][0] == '-'  && 
         argv[i][1] == 'c') ||
        
        (argv[i][0] == '-'  && 
         argv[i][1] == 'c'  && 
         argv[i][2] == 'o'  && 
         argv[i][3] == 'l'  && 
         argv[i][4] == 'o'  && 
         argv[i][5] == 'r') ) 
    {
      cmd_count++;
      core_data->style_act = false;
    }
    
    // -- modes --

    else if (argv[i][0] == '-' && 
             argv[i][1] == 'd')      // -d: definnition
    {
      cmd_count++;
      mode = SEARCH_DEFINITION;
    }

    // if none of the commands its a word, i.e. 'malloc', 'function', etc.
    else
    { 
      word_arr[word_arr_pos++] = argv[i]; 
      ERR_CHECK(word_arr_pos < WORD_ARR_MAX, "more words than the word_arr arr can hold, max is: %d\n", WORD_ARR_MAX);
    }
  }
  if (argc -cmd_count <= 1) { exit(1); } // @NOTE: no actual stuff just '-abc' type stuff
  
  // ---- keywords ----

  int found_count = 0;  // counts found matches for keyword
  
  if (HAS_FLAG(mode, SEARCH_DOCUMENTATION))
  {
    // for (int i = 0; i < word_arr_pos; ++i)
    // { P_STR(word_arr[i]); }
    doc_search_dir(core_data->sheets_path, word_arr, word_arr_pos, &found_count);
    
    // custom doc paths
    for (int i = 0; i < core_data->custom_sheet_paths_len; ++i)
    {
      // P_STR(core_data->custom_sheet_paths[i]);
      doc_search_dir(core_data->custom_sheet_paths[i], word_arr, word_arr_pos, &found_count);
    }
  }
  else if (HAS_FLAG(mode, SEARCH_DEFINITION))
  {
    if (argc - cmd_count < 2) // need dir and keyword
    {
      PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
      PF(" structure for definition search: '"); 
      PF_COLOR(PF_CYAN); PF("doc [directory] [keyword] -f"); PF_COLOR(PF_WHITE); PF("'\n");
      exit(1);
    }
    
    char* dir_name = argv[1];
    char* keyword  = argv[2];
    // P_STR(argv[0]);
    // P_STR(dir_name);
    
    char* cwd = _getcwd(NULL, 0);
    cwd += 2; // cut off the "C:" before "\directory\..."
    for (int i = 0; i < strlen(cwd); ++i)       // replace \ with /
    { cwd[i] = cwd[i] == '\\' ? '/' : cwd[i]; }
    // P_STR(cwd);
    
    char dir_path[128];
    sprintf(dir_path, "%s/%s", cwd, dir_name);
    // P_STR(dir_path);

    // remove '/' as 'def_search_dir()' appends it
    u32 len = strlen(argv[1]);
    if (argv[1][len -1] == '/' || argv[1][len -1] == '\\') 
    { argv[1][len -1] = '\0'; }

    int dir_depth = 0;
    search_result_t* results = NULL;
    def_search_dir(dir_path, keyword, &found_count, &results, &dir_depth);
    found_count = arrlen(results);  // @BUGG: n seems to be inaccurate
    // P_INT(n);
    // P_INT((int)arrlen(results));
    // for (int i = 0; i < arrlen(results); ++i)
    // {
    //   P_STR(results[i].file); 
    // }

    // @NOTE: draw bottom / top lines and results
    if (arrlen(results) > 0)  // draw top line
    {
      int w, h; io_util_get_console_size_win(&w, &h);
      w = w > MAX_LINE_WIDTH ? MAX_LINE_WIDTH : w;;
      
      PF_STYLE(PF_ITALIC, PF_PURPLE);
      PF("[%s]", keyword);
      u32 i = strlen(keyword) +2;
      while( i < w -1) { _PF("-"); i++; }
      PF_STYLE(PF_NORMAL, PF_WHITE);
      PF("\n\n");

      for (u32 i = 0; i < arrlen(results); ++i)
      {
        search_result_t* r = &results[i];
        def_print_result(r);
      }

      PF("\n");
      PF_STYLE(PF_ITALIC, PF_PURPLE);
      i = 0;
      while( i < w -1) { _PF("-"); i++; }
      PF("\n");
      PF_STYLE(PF_NORMAL, PF_WHITE);
    }
  } 

  P_INT(found_count);
  if (found_count <= 0)
  { 
    PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
    PF(" could not find keyword '"); 
    if (HAS_FLAG(mode, SEARCH_DOCUMENTATION))
    {
      PF_COLOR(PF_PURPLE);
      for (int i = 1; i < argc; ++i)
      {
        PF("%s%s",  argv[i], i < argc-1 ? ", " : "");
      }
      PF_COLOR(PF_WHITE); 
      PF("' in sheets.\n");
    }
    else if (HAS_FLAG(mode, SEARCH_DEFINITION))
    {
      PF_COLOR(PF_PURPLE);
      PF("%s", argv[3]); 
      PF_COLOR(PF_WHITE); 
      PF("' in header files.\n");
    }
    else { P_ERR("unnkown mode"); }
  }
  
  PF_COLOR(PF_WHITE);
	return 0;
}

