#include "app/doc.h"
#include "app/def.h"
#include "app/style.h"
#include "global/global.h"
#include "file_io.h"

#define STB_DS_IMPLEMENTATION   // only define once
#include "stb/stb_ds.h"

#include <ctype.h>



int main(int argc, char** argv)
{

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

    // @NOTE: '-todo' for current todo list
     if (argv[i][0] == '-' && 
         argv[i][1] == 't' &&
         argv[i][2] == 'o' &&
         argv[i][3] == 'd' &&
         argv[i][4] == 'o' )
    {
      cmd_count++;
      P("todo:\n  -> add remaining c-header doc's\n  -> add search for struct definitions\n  -> add search for function calls\n");
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
  
  bool search_mode = false;
  if (argc - cmd_count == 3) { search_mode = true; }

  // ---- keywords ----

  int n = 0;  // counts found matches for keyword
  if (search_mode)
  {
    char* directory = argv[1];
    char* keyword   = argv[2];

    // remove '/' as 'def_search_dir()' appends it
    u32 len = strlen(argv[1]);
    if (argv[1][len -1] == '/' || argv[1][len -1] == '\\') 
    { argv[1][len -1] = '\0'; }

    search_result_t* results = NULL;
    def_search_dir(directory, keyword, &n, &results);

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
    doc_search_dir("/workspace/c/term_docs/sheets/", argv[1], &n);
    // search_dir("../sheets/", argv[1], &n);
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

