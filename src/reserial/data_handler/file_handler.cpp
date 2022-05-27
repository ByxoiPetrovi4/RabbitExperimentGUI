#include "data_handler.h"

RE_OutputFiles open_folder(const char* directory, const char* subdirectory)
{
    char buf[256];
    if(opendir(directory)==0)return {0,0,0};
    RE_OutputFiles ret;
    sprintf(buf, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_LOG_FILENAME);
    ret.mainLog = fopen(buf, "w");
    sprintf(buf, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_SETTINGS_FILENAME);
    ret.settingsFile = fopen(buf, "w");
    sprintf(buf, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_SETTINGS_FILENAME);
    ret.dataFile = fopen(buf, "w");
    return ret;
}

void write_data(__re_abstract_data data, const RE_OutputFiles)
{
    /*switch (data.typ) {
        case DTAnswer:
            write_answer()
            break;
    }*/
}
