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
    DTAnswer,
    DTCommand,
    DTComment,
    DTError,
    DTUndef
#ifdef SUPPORT_MEMORY_ALLOCATION
    , DTAbstractData
#endif
};

struct __re_abstract_data
{
    void* dat;
    RE_DataType typ;
    uint8_t len;
};

typedef void (*RE_DataWrite)(const __re_abstract_data, FILE*);
typedef __re_abstract_data (*RE_DataProcess)(const char*, uint16_t&,
                                             const uint16_t);

struct RE_OutputFiles
{
    FILE* settingsFile;
    FILE* dataFile;
    FILE* mainLog;
};

//**************Messages processing**************//

__re_abstract_data process_message(const char* msg, const uint16_t len);
//out_len is length in BYTES

__re_abstract_data process_event(const char*, uint16_t&,
                                 const uint16_t);

__re_abstract_data process_answer(const char*, uint16_t&,
                                    const uint16_t);

__re_abstract_data process_command(const char*, uint16_t&,
                                   const uint16_t);

__re_abstract_data process_comment(const char*, uint16_t&,
                                    const uint16_t);

__re_abstract_data process_error(const char*, uint16_t&,
                                    const uint16_t);

//**************File data processing**************//

RE_OutputFiles open_folder(const char*);

void write_data(__re_abstract_data,
                const RE_OutputFiles);

void write_event(const __re_abstract_data, FILE*);

void write_answer(const __re_abstract_data, FILE*);

void write_command(const __re_abstract_data, FILE*);

void write_comment(const __re_abstract_data, FILE*);

void write_error(const __re_abstract_data, FILE*);

//**************Abstract convertions***************//

RE_Settings ToSettings(__re_abstract_data);

RE_DataType ToRE_DataType(const char);

char* ToStr(__re_abstract_data);

const RE_DataProcess _re_process_funcs[] = {
    process_event, process_answer, process_command, process_error};

const RE_DataWrite _re_write_funcs[] = {
    write_event, write_answer, write_command, write_error};

//**************Time functions***************//

typedef uint64_t re_time;

re_time GetTimeStamp();

void retimeToStr(const re_time, const re_time, char*);

#endif
