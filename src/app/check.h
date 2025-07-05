#ifndef CHECK_H
#define CHECK_H

#include "global/global_types.h"
#ifdef __cplusplus
extern "C" {
#endif


// @DOC: searches dir for all .sheet files, and calls doc_search_section() on them
//       dir_path: path to dir with .sheet files
//       keyword:  tag to search for in .sheet files, i.e. 'malloc', 'stdio', etc.
//       n:        used internally, to traverse the dirs
void check_search_dir(const char* dir_path, const char** keywords, int keywords_len, int* n);
bool check_search_file(const char* path, const char* file, const char** keywords, int keywords_len);

#ifdef __cplusplus
}   // extern c
#endif

#endif
