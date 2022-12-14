#include "app/doc.h"
#include "app/def.h"
#include "app/style.h"
#include "app/file_io.h"
#include "app/core_data.h"
#include "global/global.h"

#define STB_DS_IMPLEMENTATION   // only define once
#include "stb/stb_ds.h"

#include <ctype.h>
#include <direct.h>
#include <dirent/dirent.h>



int main(int argc, char** argv)
{
  core_data_t* core_data = core_data_get();

  // -- get executable name --
  // P_STR(_getcwd(NULL, 0));
  GetModuleFileName(NULL, core_data->exec_path, CORE_PATH_MAX);   // get executable location
  assert(core_data->exec_path != NULL);
  // P_STR(core_data->exec_path);
  strncpy(core_data->sheets_path, core_data->exec_path, CORE_PATH_MAX);
  assert(core_data->sheets_path != NULL);
  int dirs_walk_back = 2;
  for (u32 i = strlen(core_data->sheets_path) -1; i > 0; --i)
  {
    if (core_data->sheets_path[i] == '\\') 
    { dirs_walk_back--; if (dirs_walk_back <= 0) { break; } }
    core_data->sheets_path[i] = '\0';
  }
  strcat(core_data->sheets_path, "sheets/");
  // P_STR(core_data->sheets_path);

  if (argc < 2)
  { 
    PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
    PF(" provide keyword to search or command, i.e. '"); 
    PF_COLOR(PF_CYAN); PF("-h"); PF_COLOR(PF_WHITE);
    PF("' for help.\n"); 
    exit(1);
  }
 
  // ---- commands ----

  u32 cmd_count = 0;
  for (u32 i = 1; i < argc; ++i)
  {
    // @NOTE: '-h' or '-help' for help
    if ((argv[i][0] == '-'  && 
         argv[i][1] == 'h') ||
        
        (argv[i][0] == '-'  && 
         argv[i][1] == 'h'  && 
         argv[i][2] == 'e'  && 
         argv[i][3] == 'l'  && 
         argv[i][4] == 'p') ) 
    {
      cmd_count++;
      PF("enter: '"); 
      PF_COLOR(PF_CYAN); PF("doc word"); PF_COLOR(PF_WHITE);
      PF("' to search for all occurences of '"); 
      PF_COLOR(PF_PURPLE); PF("word"); PF_COLOR(PF_WHITE);
      PF("' in the sheets.\n");
      P("you can add sheets or alter them by going to the 'sheets' folder and adding / editing the '.sheet' files.");
      PF("use: '"); 
      PF_COLOR(PF_CYAN); PF("-f"); PF_COLOR(PF_WHITE);
      PF("' to search through '.h' files, @TODO: this\n");
    }
    
    // @NOTE: '-c' or '-color' to disable color and styles
    if ((argv[i][0] == '-'  && 
         argv[i][1] == 'c') ||
        
        (argv[i][0] == '-'  && 
         argv[i][1] == 'c'  && 
         argv[i][2] == 'o'  && 
         argv[i][3] == 'l'  && 
         argv[i][4] == 'l'  && 
         argv[i][5] == 'r') ) 
    {
      cmd_count++;
      core_data->style_act = false;
    }

    // if (argv[i][0] == '-' && 
    //     argv[i][1] == 'f')
    // { 
    //   cmd_count++;
    //   search_mode = true; 
    //   if (argc < 4)
    //   {
    //     PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
    //     PF(" structure for search: '"); 
    //     PF_COLOR(PF_CYAN); PF("doc [directory] [keyword] -f"); PF_COLOR(PF_WHITE); PF("'\n");
    //     exit(1);
    //   }
    // }
  }
  if (argc -cmd_count <= 1) { exit(1); } // @NOTE: no actual stuff just '-abc' type stuff
  
  bool search_mode = argc - cmd_count == 3; // 2(3) arguments == search_mode

  // ---- keywords ----

  int n = 0;  // counts found matches for keyword
  if (search_mode)
  {
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

    search_result_t* results = NULL;
    def_search_dir(dir_path, keyword, &n, &results);
    n = arrlen(results);  // @BUGG: n seems to be inaccurate
    // P_INT(n);
    // P_INT((int)arrlen(results));
    // for (int i = 0; i < arrlen(results); ++i)
    // {
    //   P_STR(results[i].file); 
    // }

    // @NOTE: draw bottom / top lines and results
    const int lne = 50;
    if (arrlen(results) > 0)  // draw top line
    {
      PF_COLOR(PF_PURPLE);
      PF_STYLE(PF_ITALIC);
      u32 i = strlen(keyword) +2;
      PF("[%s]", keyword);
      while (i < lne) { PF("-"); i++; }
      PF("\n");
      PF_COLOR(PF_WHITE);
      PF_STYLE(PF_NORMAL);
      PF("\n");
    }
    for (u32 i = 0; i < arrlen(results); ++i)
    {
      search_result_t* r = &results[i];
      def_print_result(r);
    }
    if (arrlen(results) > 0)  // draw bottom line
    {
      PF("\n");
      PF_COLOR(PF_PURPLE);
      u32 i = 0;
      while (i < lne) { PF("-"); i++; }
      PF("\n");
      PF_COLOR(PF_WHITE);
    }
  }
  else
  {
    doc_search_dir(core_data->sheets_path, argv[1], &n);
    // doc_search_dir("/workspace/c/term_docs/sheets/", argv[1], &n);
    // doc_search_dir("../sheets/", argv[1], &n);
  }

  if (n <= 0)
  { 
    PF_COLOR(PF_RED); PF("[!]"); PF_COLOR(PF_WHITE);  
    PF(" could not find keyword '"); 
    PF_COLOR(PF_PURPLE); PF("%s", search_mode ? argv[2] : argv[1]); PF_COLOR(PF_WHITE); 
    
    if (search_mode) { PF("' in header files.\n"); }
    else             { PF("' in sheets.\n"); }
  }
  
  PF_COLOR(PF_WHITE);
	return 0;
}

