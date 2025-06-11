#ifndef DOC_H
#define DOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

// @DOC: searches dir for all .sheet files, and calls doc_search_section() on them
//       dir_path: path to dir with .sheet files
//       keyword:  tag to search for in .sheet files, i.e. 'malloc', 'stdio', etc.
//       n:        used internally, to traverse the dirs
void doc_search_dir(const char* dir_path, const char** keywords, int keywords_len, int* n);
bool doc_search_file(const char* path, const char* file, const char** keywords, int keywords_len);
void doc_print_section(char* sec, int sec_len, const char** keywords, int keywords_len, const char* file, const int line);
void doc_color_code_section(char* sec, int len);
void doc_color_code_escape_chars(char* sec, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr);
void doc_color_code_color_codes(char* sec, int sec_len, char* buf, int* buf_pos_ptr, int* i_ptr, bool* skip_char_ptr, bool* syntax);
void doc_color_code_color_codes_print_file(const char* box_indent, int box_width, int box_height, bool print_line_nr, const char* name, int name_len, const char* path);

// @DOC: count lines in directory of .sheet files
void doc_count_lines_dir(const char* dir_path, int* lines);
void doc_count_lines_file(const char* path, int* lines);

#ifdef __cplusplus
}   // extern c
#endif

#endif
