#ifndef DOC_H
#define DOC_H

#include "global/global.h"

// @DOC: searches dir for all .sheet files, and calls doc_search_section() on them
//       dir_path: path to dir with .sheet files
//       keyword:  tag to search for in .sheet files, i.e. 'malloc', 'stdio', etc.
//       n:        used internally, to traverse the dirs
void doc_search_dir(const char* dir_path, const char** keywords, int keywords_len, int* n);
bool doc_search_file(const char* path, const char* file, const char** keywords, int keywords_len);
void doc_print_section(char* sec, const char* keyword, const char* file);
void doc_color_code_section(char* section);
void doc_color_code_escape_chars(char* sec, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr);
void doc_color_code_color_codes(char* sec, int sec_len, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr, bool* syntax);

#endif
