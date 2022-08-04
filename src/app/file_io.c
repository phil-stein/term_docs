#include "file_io.h"
#include <direct.h>


bool check_file_exists(const char* file_path)
{
    FILE* f = fopen(file_path, "rb");
    if (f == NULL) {
        return false;
    }
    return true;
}

char* read_text_file(const char* file_path)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    len++;   // for null-terminator

    // alloc memory 
    text = calloc(1, len * sizeof(char));
    assert(text != NULL);
    
    // fill text buffer
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);
    text[len -1] = '\0'; 

    return text;
}
char* read_text_file_len(const char* file_path, int* length)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    len++;   // for null-terminator

    // alloc memory 
    text = calloc(1, len);
    assert(text != NULL);

    // fill text buffer
    fread(text, sizeof(char), len, f);
    assert(strlen(text) > 0);
    fclose(f);
    text[len -1] = '\0'; 

    *length = len;
    return text;
}

void write_text_file(const char* file_path, const char* txt, int len)
{
    FILE* f;

    f = fopen(file_path, "wb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(false);
    }

    int rtn = fwrite(txt, sizeof(char), len, f);
    assert(rtn != EOF);
    // fprintf(f, "%s", txt);

    fclose(f);
}






