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
        sprintf(output_buffer, "NoEnd");
        output.len = strlen(output_buffer);
        output.typ = DTError;
        pos++;
        return output;
    }
    output.typ = DTEvent;
    switch(str[pos])
    {
        case EBeep:
            sprintf(output_buffer, "Sound %d", (int)EBeep);
            output.len = strlen(output_buffer);
            break;
        case EDrop:
            sprintf(output_buffer, "Drop %d", (int)EDrop);
            output.len = strlen(output_buffer);
            break;
        case EPress:
            sprintf(output_buffer, "Press %d", (int)EPress);
            output.len = strlen(output_buffer);
            break;
        case EIncPress:
            sprintf(output_buffer, "WrongPress %d", (int)EIncPress);
            output.len = strlen(output_buffer);
            break;
        case ENoPress:
            sprintf(output_buffer, "NoPress %d", (int)ENoPress);
            output.len = strlen(output_buffer);
            break;
        case ETimeout:
            sprintf(output_buffer, "Timeout %d", (int)ETimeout);
            output.len = strlen(output_buffer);
            break;
        case ECorrect:
            sprintf(output_buffer, "Correct %d", (int)ECorrect);
            output.len = strlen(output_buffer);
            break;
        default:
            sprintf(output_buffer, "NoEvent -1");
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
        sprintf(output_buffer, "Sound %d", (int)KBeep);
        output.len = strlen(output_buffer);
        break;
    case KPause:
        sprintf(output_buffer, "Pause %d", (int)KPause);
        output.len = strlen(output_buffer);
        break;
    case KFeed:
        sprintf(output_buffer, "Feed %d", (int)KFeed);
        output.len = strlen(output_buffer);
        break;
    case KEFeed:
        sprintf(output_buffer, "EFeed %d", (int)KEFeed);
        output.len = strlen(output_buffer);
        break;
    case KStart:
        if(experiment_start==0)experiment_start = GetTimeStamp();
        sprintf(output_buffer, "Start %d", (int)KStart);
        output.len = strlen(output_buffer);
        break;
    case KClose:
        sprintf(output_buffer, "Close %d", (int)KClose);
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
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

void retimeToStr(const re_time current, const re_time start, char* str)
{
    re_time t = current - start;
    sprintf(str, "%lu:%lu:%lu.%lu", (t/3600000000)%24, (t/60000000)%60, (t/1000000)%60, (t/1000)%1000);
}
