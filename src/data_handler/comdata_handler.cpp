#include "data_handler.h"
#include <cstring>

char output_buffer[RE_BUFFER_SIZE];

__re_abstract_data process_event(const char* str, uint16_t &pos,
                                 const uint16_t len)
{
    __re_abstract_data output;
    output.dat = output_buffer;
    if(str[pos+1] != End || pos + 2 > len)
    {
        strcpy(output_buffer, "NoEnd");
        output.len = strlen(output_buffer);
        output.typ = DTError;
        pos++;
        return output;
    }
    output.typ = DTEvent;
    switch(str[pos])
    {
        case ESound:
            strcpy(output_buffer, "Sound");
            output.len = strlen(output_buffer);
            break;
        case EDrop:
            strcpy(output_buffer, "Drop");
            output.len = strlen(output_buffer);
            break;
        case EPress:
            strcpy(output_buffer, "Press");
            output.len = strlen(output_buffer);
            break;
        case ETimeout:
            strcpy(output_buffer, "Timeout");
            output.len = strlen(output_buffer);
            break;
        case ECorrect:
            strcpy(output_buffer, "Correct");
            output.len = strlen(output_buffer);
            break;
        default:
            strcpy(output_buffer, "NoEvent");
            output.len = strlen(output_buffer);
            output.typ = DTError;
    }
    pos += 2;
    return output;
}

__re_abstract_data process_keyword(const char*, uint16_t&, const uint16_t){}
__re_abstract_data process_settings(const char*, uint16_t&, const uint16_t){}

void write_event(const __re_abstract_data, FILE*){}
void write_keyword(const __re_abstract_data, FILE*){}
void write_settings(const __re_abstract_data, FILE*){}

char* ToStr(__re_abstract_data dat)
{
    return (char*)dat.dat;
}
