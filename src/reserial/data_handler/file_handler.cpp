#include "data_handler.h"

RE_OutputFiles open_folder(const char* directory, const char* subdirectory)
{
    char buf[256];
    RE_OutputFiles ret;
    if(directory[0] == '\0' || subdirectory[0] == '\0')
    {
        ret.mainLog = fopen(RE_DEFAULT_LOG_FILENAME, "w");
        ret.settingsFile = fopen(RE_DEFAULT_SETTINGS_FILENAME, "w");
        ret.dataFile = fopen(RE_DEFAULT_DATA_FILENAME, "w");
        return ret;
    }
    if(opendir(directory)==0)return {0,0,0};
    struct stat st = {0};
    snprintf(buf, 256, "%s/%s", directory, subdirectory);
    if(stat(buf, &st) == -1)mkdir(buf, 0700);   //check if dir is not existing than make
    snprintf(buf, 256, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_LOG_FILENAME);
    ret.mainLog = fopen(buf, "w");
    snprintf(buf, 256, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_SETTINGS_FILENAME);
    ret.settingsFile = fopen(buf, "w");
    snprintf(buf, 256, "%s/%s/%s", directory, subdirectory, RE_DEFAULT_DATA_FILENAME);
    ret.dataFile = fopen(buf, "w");
    return ret;
}
