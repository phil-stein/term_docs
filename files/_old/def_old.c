
// old c highlighting
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
