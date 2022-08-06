#ifndef STYLE_H
#define STYLE_H

#include "global/global.h"

#define BORDER_CHAR '|'

#define COL_TAG     PF_CYAN 
#define COL_WARNING PF_RED  
#define COL_EXAMPLE PF_CYAN 
#define COL_INFO    PF_PURPLE 
#define COL_LINK    PF_BLUE 

#define COL_COMMENT PF_WHITE 
#define COL_FUNC    PF_BLUE 
#define COL_TYPE    PF_GREEN 
#define COL_VALUE   PF_YELLOW
#define COL_MACRO   PF_PURPLE

// pass the txt to be modified as well as the buffer the sections from txt
// are written to, also pointers to the buffer position and i (iterator)
// of the for loop in which this should be called
// e.g.
//    char* txt = load_file()
//    char  buf[512];
//    int   buf_pos = 0;
//    for (int i = 0; i < len -1; ++i)
//    {
//      // ...
//      style_highlight_c(txt, buf, &buf_pos, &i)
//     }
void style_highlight_c(char* txt, char* buf, int* buf_pos_ptr, int* i_ptr); 


#endif
