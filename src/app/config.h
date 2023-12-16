#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


#include "global/global.h"

void config_read_config_file(const char* path);
void config_handle_argument();
int config_parse_bool();


#ifdef __cplusplus
}   // extern c
#endif

#endif // CONFIG_H
