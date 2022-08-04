#ifndef STYLE_H
#define STYLE_H

#include "global/global.h"

#define BORDER_CHAR '|'

#define COL_TAG     PF_CYAN 
#define COL_WARNING PF_RED 
#define COL_FUNC    PF_BLUE 
#define COL_TYPE    PF_GREEN 
#define COL_VALUE   PF_YELLOW 
#define COL_EXAMPLE PF_CYAN 
#define COL_COMMENT PF_WHITE 
#define COL_INFO    PF_PURPLE 
#define COL_LINK    PF_BLUE 

// pass the txt to be modified as well as the buffer the sections from txt
// are written to, also pointers to the buffer position and i (iterator)
// of the for loop in which this should be called
// e.g.
//    for (u32 i = 0; i < len -1; ++i)
//    {
//      // ...
//      style_highlight_c(...)
//     }
void style_highlight_c(char** txt_ptr, char** buf_ptr, u32* buf_pos_ptr, u32* i_ptr);


#endif
