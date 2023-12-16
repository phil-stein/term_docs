#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global/global.h"


// returns true if the file under the specified path "file_path" exists, otherwise false
bool check_file_exists(const char* file_path);

// read a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* read_text_file(const char* file_path);
// read the given lengt of a text-file and output all contents as a char*
// !!! free() the returned char* as it gets allocated
// taken from: https://github.com/jdah/minecraft-weekend/blob/master/src/gfx/shader.c
char* read_text_file_len(const char* file_path, int* length);

// writes text "txt" into file at "file_path", creates file if it doesnt exist
// "len" is the length of "txt", or shorter if you want to cut off the string
void write_text_file(const char* file_path, const char* txt, int len);

#ifdef __cplusplus
}   // extern c
#endif

#endif
