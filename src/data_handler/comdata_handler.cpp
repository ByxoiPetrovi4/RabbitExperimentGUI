#include "data_handler.h"

__re_abstract_data process_event(const char* str)
{
    if(str[1] != End)
    switch(str[0])
    {
        case ESound:
            break;
        case EDrop:
            break;
        case EPress:
            break;
        case ETimeout:
            break;
        case ECorrect:
            break;
        default:
            1==1;
    }
}
