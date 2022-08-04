#ifndef DOC_H
#define DOC_H

#include "global/global.h"

void doc_search_dir(const char* dir_path, const char* keyword, int* n);
bool doc_search_section(const char* path, const char* file, const char* keyword);
void doc_print_section(char* sec, const char* keyword, const char* file);
void doc_color_code_section(char* section);

#endif
