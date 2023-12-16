// old highlight system
    // if (isspace(sec[i -1]) && sec[i -1] != '|') //  && !in_tag)  // !isalnum(sec[i -1])
    // {

    //   // -- types --
    //   for (u32 s = 0; s < key_types_len; ++s) // string
    //   {
    //     // @UNSURE: could use strcmp()
    //     bool equal = false;
    //     u32 len = strlen(key_types[s]);
    //     u32 c;
    //     for (c = 0; c < len; ++c)             // char
    //     {
    //       equal = sec[i +c] == key_types[s][c];
    //       if (!equal) { break; }
    //     }
    //     if (equal && TYPE_END(sec[i +len]))
    //     {
    //       DUMP_COLORED(len, COL_TYPE);
    //       break;
    //     }
    //   }

    //   // -- flow control --
    //   for (u32 s = 0; s < key_flow_ctrl_len; ++s) // string
    //   {
    //     // @UNSURE: could use strcmp()
    //     bool equal = false;
    //     u32 len = strlen(key_flow_ctrl[s]);
    //     u32 c;
    //     for (c = 0; c < len; ++c)             // char
    //     {
    //       equal = sec[i +c] == key_flow_ctrl[s][c];
    //       if (!equal) { break; }
    //     }
    //     if (equal && FLOW_CTRL_END(sec[i +len]))
    //     {
    //       DUMP_COLORED(len, COL_TYPE);
    //       break;
    //     }
    //   }
    // }

    // // -- functions --
    // int j = 0;
    // while(isalnum(sec[i +j]) || sec[i +j] == '_') 
    // { j++; }
    // if (sec[i +j] == '(')
    // {
    //   BUF_DUMP();
    //   SET_COLOR(COL_FUNC);
    //   while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
    //   BUF_DUMP();
    //   SET_COLOR(PF_WHITE);
    // }

    // // -- values --

    // if (isdigit(sec[i]) && !isalpha(sec[i -1]) && 
    //     (!isalpha(sec[i +1]) || sec[i +1] == 'f') && 
    //     !in_tag)                                      // numbers
    // {
    //   BUF_DUMP();
    //   SET_COLOR(COL_VALUE);
    //   int j = 0;
    //   while (isdigit(sec[i +j]) || sec[i +j] == '.' || sec[i +j] == 'f') { j++; }
    //   while (j > 0) { buf[buf_pos++] = sec[i++]; j--; }
    //   BUF_DUMP();
    //   SET_COLOR(PF_WHITE);
    // }
    // if (sec[i +0] == 't' && sec[i +1] == 'r' &&     // value symbols
    //     sec[i +2] == 'u' && sec[i +3] == 'e')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (sec[i +0] == 'f' && sec[i +1] == 'a' &&
    //     sec[i +2] == 'l' && sec[i +3] == 's' &&
    //     sec[i +4] == 'e')
    // { DUMP_COLORED(5, COL_VALUE); }
    // if (sec[i +0] == 'N' && sec[i +1] == 'U' &&
    //     sec[i +2] == 'L' && sec[i +3] == 'L')
    // { DUMP_COLORED(4, COL_VALUE); }
    // if (sec[i] == '"')                        // strings
    // {
    //   BUF_DUMP();
    //   SET_COLOR(COL_VALUE);
    //   buf[buf_pos++] = sec[i++];
    //   while (sec[i] != '"') { buf[buf_pos++] = sec[i++]; }
    //   buf[buf_pos++] = sec[i++];
    //   BUF_DUMP();
    //   SET_COLOR(PF_WHITE);
    // }
    // if (sec[i] == '\'' && sec[i +2] == '\'')  // chars
    // {
    //   BUF_DUMP();
    //   SET_COLOR(COL_VALUE);
    //   buf[buf_pos++] = sec[i++];
    //   buf[buf_pos++] = sec[i++];
    //   buf[buf_pos++] = sec[i++];
    //   BUF_DUMP();
    //   SET_COLOR(PF_WHITE);
    // }



// old keyword highlight system
//
      // if (sec[i +0] == 'v' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'i' && sec[i +3] == 'd' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'i' && sec[i +1] == 'n' &&
      //     sec[i +2] == 't' && TYPE_END(sec[i +3]))
      // { DUMP_COLORED(3, COL_TYPE); }
      // if (sec[i +0] == 'f' && sec[i +1] == 'l' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'a' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 'd' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'u' && sec[i +3] == 'b' &&
      //     sec[i +4] == 'l' && sec[i +5] == 'e' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 'h' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'r' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 'l' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'n' && sec[i +3] == 'g' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 'i' &&
      //     sec[i +2] == 'z' && sec[i +3] == 'e' &&
      //     sec[i +4] == '_' && sec[i +5] == 't' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 'u' && sec[i +1] == 'n' &&
      //     sec[i +2] == 's' && sec[i +3] == 'i' &&
      //     sec[i +4] == 'g' && sec[i +5] == 'n' &&
      //     sec[i +6] == 'e' && sec[i +7] == 'd' && 
      //     TYPE_END(sec[i +8]))
      // { DUMP_COLORED(8, COL_TYPE); }
      // if (sec[i +0] == 'e' && sec[i +1] == 'n' &&
      //     sec[i +2] == 'u' && sec[i +3] == 'm' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 's' && sec[i +1] == 't' &&
      //     sec[i +2] == 'r' && sec[i +3] == 'u' &&
      //     sec[i +4] == 'c' && sec[i +5] == 't' && 
      //     TYPE_END(sec[i +6]))
      // { DUMP_COLORED(6, COL_TYPE); }
      // if (sec[i +0] == 'c' && sec[i +1] == 'h' &&
      //     sec[i +2] == 'a' && sec[i +3] == 'r' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'b' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'o' && sec[i +3] == 'l' && 
      //     TYPE_END(sec[i +4]))
      // { DUMP_COLORED(4, COL_TYPE); }
      // if (sec[i +0] == 'c' && sec[i +1] == 'o' &&
      //     sec[i +2] == 'n' && sec[i +3] == 's' &&
      //     sec[i +4] == 't' && TYPE_END(sec[i +5]))
      // { DUMP_COLORED(5, COL_TYPE); }
      // if (sec[i +0] == 't' && sec[i +1] == 'y' &&
      //     sec[i +2] == 'p' && sec[i +3] == 'e' &&
      //     sec[i +4] == 'd' && sec[i +5] == 'e' &&
      //     sec[i +6] == 'f' && TYPE_END(sec[i +7]))
      // { DUMP_COLORED(7, COL_TYPE); }
 



