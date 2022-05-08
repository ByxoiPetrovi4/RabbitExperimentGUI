#ifndef RE_DATA_HANDLER
#define RE_DATA_HANDLER

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <ctime>

#include "protocol.h"

#define RE_BUFFER_SIZE                  0x7f

#define RE_DEFAULT_SETTINGS_FILENAME    "settings.txt"
#define RE_DEFAULT_DATA_FILENAME        "data.csv"
#define RE_DEFAULT_LOG_FILENAME         "main.log"

enum RE_DataType : char
{
    DTEvent,
    DTKeyword,
    DTSettings,
    DTError = 0x7f
};

struct __re_abstract_data
{
    void* dat;
    RE_DataType typ;
    uint8_t len;
};

typedef void (*RE_DataWrite)(const __re_abstract_data, FILE*);
typedef __re_abstract_data (*RE_DataProcess)(const char*);

struct RE_OutputFiles
{
    FILE* settingsFile;
    FILE* dataFile;
    FILE* mainLog;
};

//**************Messages processing**************//

__re_abstract_data process_message(const char* msg, const uint16_t len);
//out_len is length in BYTES

__re_abstract_data process_event(const char*);

__re_abstract_data process_keyword(const char*);

__re_abstract_data process_settings(const char*);
//**************File data processing**************//

RE_OutputFiles open_folder(const char*);

void write_data(__re_abstract_data,
                const RE_OutputFiles);

void write_event(const __re_abstract_data, FILE*);

void write_keyword(const __re_abstract_data, FILE*);

void write_settings(const __re_abstract_data, FILE*);

//**************Abstract convertions***************//

RE_Settings RE_Settings(__re_abstract_data);

const RE_DataProcess _re_process_funcs[] = {
    process_event, process_keyword, process_settings};

const RE_DataWrite _re_write_funcs[] = {
    write_event, write_keyword, write_settings};

#endif
