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
 


/*
// @TODO: give array of keyword to check if has multiple at once
bool doc_search_file_01(const char* path, const char* file, const char** keywords, int keywords_len)
{
  if (!check_file_exists(path)) { return false; }
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);

  bool found_one = false;
  for (int i = 0; i < keywords_len; ++i)
  {
    char* keyword = keywords[i];

    // @TODO: see if better way to do this
    // bool found_one = false;
    bool found = false;
    u32 found_count = 0;
    int start;
    for (start = 1; txt[start] != '\0'; start++)
    {
      if ((txt[start] == keyword[0] || 
            txt[start] == '\\') && 
          txt[start -1] == BORDER_CHAR)
      {
        found = true;
        int pos = 0;
        int skipped = 0;
        for (pos = 0; keyword[pos] != '\0'; pos++)
        {
          int j = pos + skipped;

          // escaped #, also need to j++; because adding skipped to j before
          if (txt[start +j] == '\\' && txt[start +j +1] == '#') 
          { skipped++; j++; }

          if (txt[start +j] != keyword[pos])
          { found = false; break; }
        }
        if (txt[start +pos +skipped] != BORDER_CHAR) 
        { found = false; }
      }
      if (found) 
      {
        P("found"); 
        // -- section start & end --
        while (txt[start] != '#' || txt[start -1] == '\\') { start--; }
        int end = start +1;
        while (txt[end] != '#' || txt[end -1] == '\\')   { end++; }

        start++;          // skip '#'
        char end_char = txt[end];
        txt[end] = '\0';  // shorten txt to section

        doc_print_section(txt + start, keyword, file);
        txt[end] = end_char;
        start = end +1;
        found_one = true;
        found = false;
        found_count++;
      }
    }

    // print ending line
    if (found_count > 0)
    {
      int w, h; io_util_get_console_size_win(&w, &h);
      w = w > MAX_LINE_WIDTH ? MAX_LINE_WIDTH : w;;
      
      PF_COLOR(PF_PURPLE);
      int i = 0;
      while( i < w -1) { _PF("-"); i++; }
      
      PF("\n");
      PF_COLOR(PF_WHITE);
    }
  }
  FREE(txt);

  return found_one;
}

// @TODO: see if better way to do this
bool doc_search_file_02(const char* path, const char* file, const char** keywords, int keywords_len)
{
  if (!check_file_exists(path)) { return false; }
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);

  bool found_one = false;
  bool found = false;
  int sec_start = 0;
  #define TAG_MAX 128
  char tag[TAG_MAX];
  int  tag_pos = 0;
  int keywords_matched_count = 0;
  u32 found_count = 0;
  for (int i = 0; i < keywords_len; ++i)
  {
    char* keyword = keywords[i];

    // bool found_one = false;
    int start;
    P_LINE();
    for (start = 1; txt[start] != '\0'; start++)
    {
      if ((txt[start] == keyword[0] || 
            txt[start] == '\\') && 
          txt[start -1] == BORDER_CHAR)
      {
        // write txt section into string with escaped chars and compare afterwards

        // found = true;
        tag_pos = 0;
        int pos = 0;
        int skipped = 0;
        for (pos = 0; keyword[pos] != '\0'; pos++)
        {
          int j = pos + skipped;

          // escaped #, also need to j++; because adding skipped to j before
          if (txt[start +j] == '\\' && txt[start +j +1] == '#') 
          { skipped++; j++; }

          if (txt[start +j] != keyword[pos])
          { found = false; break; }

          ERR_CHECK(pos < TAG_MAX, "checking tag that is longer than the 'tag' string\n");
          tag[pos] = txt[start +j +skipped];
        }
        tag[pos] = '\0';
        // check if end in | BORDER_CHAR
        if (txt[start +pos +skipped] != BORDER_CHAR) 
        { found = false; }
        // check if tag == keyword
        if (strcmp(tag, keyword) == 0)
        {
          P_STR(tag);
          P("tag == keyword");
          // found = true;
          keywords_matched_count++;
          P_INT(keywords_matched_count);
          sec_start = start;
        }
      }

    }
  }

  // if (found && keywords_matched_count == keywords_len) 
  if (keywords_matched_count == keywords_len) 
  {
    P_INT(keywords_matched_count);
    P_INT(keywords_len);
    P_BOOL(found);
    P_INT(sec_start);
    // -- section start & end --
    while (txt[sec_start] != '#' || txt[sec_start -1] == '\\') { sec_start--; }
    int end = sec_start +1;
    while (txt[end] != '#' || txt[end -1] == '\\')   { end++; }

    sec_start++;          // skip '#'
    char end_char = txt[end];
    txt[end] = '\0';  // shorten txt to section

    // @TODO: make this use all of keywords
    doc_print_section(txt + sec_start, keywords[0], file);
    txt[end] = end_char;
    sec_start = end +1;
    found_one = true;
    found = false;
    found_count++;
  }

  // print ending line
  if (found_count > 0)
  {
    int w, h; io_util_get_console_size_win(&w, &h);
    w = w > MAX_LINE_WIDTH ? MAX_LINE_WIDTH : w;;

    PF_COLOR(PF_PURPLE);
    int i = 0;
    while( i < w -1) { _PF("-"); i++; }

    PF("\n");
    PF_COLOR(PF_WHITE);
  }
  FREE(txt);

  return found_one;
}
*/
