#ifndef _DATA_HANDLER_
#define _DATA_HANDLER_

#include "protocol.h"
#include <inttypes.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>

#define _MASTER_
#ifdef _MASTER_
#define RE_BUFFER_SIZE                  0x7f

#define RE_DEFAULT_SETTINGS_FILENAME    "settings.txt"
#define RE_DEFAULT_DATA_FILENAME        "data.csv"
#define RE_DEFAULT_LOG_FILENAME         "main.log"

struct RE_OutputFiles
{
    FILE* settingsFile;
    FILE* dataFile;
    FILE* mainLog;
};

#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#endif

enum RE_DataType : char
{
    DTEvent,
    DTAnswer,
    DTCommand,
    DTComment,
    DTError,
    DTUndef,
    DTSettings
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

typedef __re_abstract_data (*RE_DataProcess)(const char*, uint16_t&,
                                             const uint16_t);
typedef void (*RE_DataWrite)(const __re_abstract_data, FILE*);

//void (*myResetFunction)(void) = 0;

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

#ifdef _MASTER_
//**************File data processing**************//

RE_OutputFiles open_folder(const char*, const char*);

void write_data(__re_abstract_data,
                const RE_OutputFiles);
void write_event(const __re_abstract_data, FILE*);
void write_answer(const __re_abstract_data, FILE*);
void write_command(const __re_abstract_data, FILE*);
void write_comment(const __re_abstract_data, FILE*);
void write_error(const __re_abstract_data, FILE*);

//**************Time functions***************//

typedef uint64_t re_time;

re_time GetTimeStamp();

void retimeToStr(const re_time, const re_time, char*);

#endif

//**************Abstract convertions***************//

RE_Settings StrToSettings(const char*);

void SettingsToStr(RE_Settings, char*);

RE_Settings ToSettings(__re_abstract_data);

RE_DataType ToRE_DataType(const char);

char* ToStr(__re_abstract_data);

#ifdef _CONTROLLER_

eEvent ToeEvent(__re_abstract_data);

#endif

const RE_DataProcess _re_process_funcs[] = {
    process_event, process_answer, process_command, process_comment, process_error};

#ifdef _MASTER_

const RE_DataWrite _re_write_funcs[] = {
    write_event, write_answer, write_command, write_comment, write_error};

#endif

#endif
