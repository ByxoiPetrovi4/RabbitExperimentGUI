#include "data_handler.h"

static char output_buffer[RE_BUFFER_SIZE];
static re_time experiment_start = 0;

__re_abstract_data process_message(const char* msg, const uint16_t len)
{
    uint16_t pos = 1;
    enum RE_DataType dp = ToRE_DataType(msg[0]);
    if(dp==DTUndef)
    {
        __re_abstract_data output;
        output.dat = output_buffer;
        strcpy(output_buffer, "UnknownSSymbol");
        output.len = strlen(output_buffer);
        output.typ = DTError;
        pos++;
        return output;
    }
    return _re_process_funcs[dp](msg, pos, len);
}

__re_abstract_data process_event(const char* str, uint16_t &pos,
                                 const uint16_t len)
//Master received event message type
//That type of message doesn't need answer
//This function translit that message to __re_abstract string
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
        case EBeep:
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
        case EIncPress:
            strcpy(output_buffer, "Wrong press");
            output.len = strlen(output_buffer);
            break;
        case ENoPress:
            strcpy(output_buffer, "No Press");
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

__re_abstract_data process_answer(const char* str, uint16_t& pos, const uint16_t len)
//Master received answer message type
//That type of message doesn't need answer
//This function translit some type of answers
{
    __re_abstract_data output;
    output.dat = output_buffer;
    output.typ = DTAnswer;
    switch(str[pos])
    {
    case KSettings:
        output.typ = DTSettings;
        break;
    case KBeep:
        strcpy(output_buffer, "Sound");
        output.len = strlen(output_buffer);
        break;
    case KPause:
        strcpy(output_buffer, "Pause");
        output.len = strlen(output_buffer);
        break;
    case KFeed:
        strcpy(output_buffer, "Feed");
        output.len = strlen(output_buffer);
        break;
    case KEFeed:
        strcpy(output_buffer, "EFeed");
        output.len = strlen(output_buffer);
        break;
    case KStart:
        if(experiment_start==0)experiment_start = GetTimeStamp();
        strcpy(output_buffer, "Start");
        output.len = strlen(output_buffer);
        break;
    case KClose:
        strcpy(output_buffer, "Close");
        output.len = strlen(output_buffer);
    default:
        output.typ = DTAnswer;
    }
    return output;
}
__re_abstract_data process_command(const char*, uint16_t&, const uint16_t)
{

}
__re_abstract_data process_comment(const char* str, uint16_t& pos, const uint16_t len)
{
    __re_abstract_data output;
    output.dat = output_buffer;
    output.typ = DTComment;
    strcpy(output_buffer, str + pos);
    output.len = strlen(output_buffer);
    return output;
}
__re_abstract_data process_error(const char*, uint16_t&, const uint16_t){}

void write_event(const __re_abstract_data, FILE*){}
void write_answer(const __re_abstract_data, FILE*){}
void write_command(const __re_abstract_data, FILE*){}
void write_comment(const __re_abstract_data, FILE*){}
void write_error(const __re_abstract_data, FILE*){}

char* ToStr(__re_abstract_data dat)
{
    return (char*)dat.dat;
}

RE_DataType ToRE_DataType(const char s)
{
    switch(s)
    {
        case Event:
            return DTEvent;
        case Answer:
            return DTAnswer;
        case Command:
            return DTCommand;
        case Comment:
            return DTComment;
        case Error:
            return DTError;
        default:
            return DTUndef;
    }
    return DTUndef;
}

RE_Settings StrToSettings(const char* input)
{
    RE_Settings out;
    sscanf(input, "%*[$]%d%*[$]%d%*[$]%d%*[$]%d%*[$]%d%*[$]%d", &out.food,
            &out.press_interval, &out.sound_length, &out.manual, &out.min_delay,
            &out.max_delay);
    return out;
}

void SettingsToStr(RE_Settings settings, char* output)
{
    sprintf(output, "$%d$%d$%d$%d$%d$%d\n", settings.food,
            settings.press_interval, settings.sound_length, settings.manual,
            settings.min_delay,  settings.max_delay);
}

RE_Settings ToSettings(__re_abstract_data dat)
{
    RE_Settings out = {0,0,0,0,0,0};
    if(dat.typ != DTSettings)return out;
    memcpy(&out, dat.dat, dat.len);
    return out;
}

re_time GetTimeStamp()
{
    struct timeval tv;
    mingw_gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

void retimeToStr(const re_time current, const re_time start, char* str)
{
    re_time t = current - start;
    sprintf(str, "%llu:%llu:%llu.%llu", (t/3600000000)%24, (t/60000000)%60, (t/1000000)%60, (t/1000)%1000);
}
