#ifndef CONFIG_H
#define CONFIG_H

// #include <vcruntime.h>
#ifdef __cplusplus
extern "C" {
#endif


#include "global/global.h"

typedef enum
{
  CONFIG_ERROR_NO_ERROR       = 0,
  CONFIG_ERROR_INVALID_TYPE   = FLAG(1),
  CONFIG_ERROR_INVALID_STRING = FLAG(2),
  CONFIG_ERROR_INVALID_NUMBER = FLAG(3),

} config_error_flag;
INLINE void config_print_config_error_flag(config_error_flag state, const char* name, const char* _file, const char* _func, const int _line)
{
  PF_COLOR(PF_CYAN); _PF("%s", name); PF_STYLE_RESET(); _PF(":\n");
  
  if (state == CONFIG_ERROR_NO_ERROR)
  {
    PF("CONFIG_ERROR_NO_ERRO");
  }
  else 
  {
    P_FLAG_MEMBER(state, CONFIG_ERROR_INVALID_TYPE);
    P_FLAG_MEMBER(state, CONFIG_ERROR_INVALID_STRING);
    P_FLAG_MEMBER(state, CONFIG_ERROR_INVALID_NUMBER);
  }
  _PF("\n");
  _PF_IF_LOC(_file, _func, _line);
}
#define P_CONFIG_ERROR_FLAG(v)  config_print_config_error_flag((v), #v, __FILE__, __func__, __LINE__)

typedef enum
{
  ARGUMENT_TYPE_INVALID,
  ARGUMENT_TYPE_STR,
  ARGUMENT_TYPE_BOOL,
  ARGUMENT_TYPE_NUMBER,
  ARGUMENT_TYPE_COLOR,

} argument_type;

#define ARGUMENT_T_STR_MAX 512
typedef struct
{
  argument_type type;
  union
  {
    char _str[ARGUMENT_T_STR_MAX];
    bool _bool;
    int  color;
    f32  number;
  };
  int _str_len;
} argument_t;
#define ARGUMENT_T_INIT() { ._str_len = 0, ._str = "" }

// void config_read_config_file(const char* path);
void config_read_config_file(const char* path, bool print_config);
// void config_handle_argument();
void config_handle_argument(argument_t* arg);

void config_handle_group();

config_error_flag config_parse_argument(argument_t* arg, char* buf, int buf_len);
int config_parse_bool();


#ifdef __cplusplus
}   // extern c
#endif

#endif // CONFIG_H
