#ifndef DEF_H
#define DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"

#define SEARCH_RESULT_FILE_MAX 128
#define SEARCH_RESULT_TXT_MAX  512

typedef struct search_result_t
{
  u32 start;
  u32 end;
  u32 lne;
  char file[SEARCH_RESULT_FILE_MAX];
  char txt[SEARCH_RESULT_TXT_MAX];
}search_result_t;

void def_print_result(search_result_t* r);
void def_search_dir(const char* dir_path, const char* keyword, int* n, search_result_t** search_results, int* dir_depth);
bool def_search_section(const char* path, const char* file, const char* keyword, search_result_t** search_results);
void def_search_and_print(char* dir_name, char* keyword, int* found_count);

#ifdef __cplusplus
}   // extern c
#endif

#endif
