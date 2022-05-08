#ifndef RE_PROTOCOL
#define RE_PROTOCOL

#include <cstdint>
#include <cstring>
#include <cmath>

#include "setup_header.h"

#define ER_START_COMMUNICATION '~'
//Communications starts when after 3 start symbols
//master will reciece 3 start symbols as answer


//Keywords for protocol
enum Keywords : char
{
    KSettings    = 's',
    KSound       = 'b',
    KPause       = 'p',
    KFeed        = 'f',
    KEFeed       = 'e',
    KStart       = 'o',
    KClose       = 'c'
#ifdef DEBUG_COMMANDS_ON
    , Report    = 'r'
    , Terminate = 't'
    , ChangeGrid= 'g'
#endif
};

enum Events : char
{
    ESound      = 'b',
    EDrop       = 'd',
    EPress      = 'x',
    ETimeout    = 'y',
    ECorrect    = 'z'
};

enum SSymbols : char
{
    Command     = ':',
    Answer      = ';',
    Event       = '&',
    End         = '\n',
    Delimetr    = '$',
    Comment     = '#',
    Error       = '!'
};


//Protocol work example
//T: ~~~ R: ~~~
//T: :s$data1$data2\n   R: ;s$data1$data2\n
//T: :o\n               R: ;o\n
//R: #allfine\n
//T: :p                 R: !0\n
//No end symbol error on command received or invalid command error = 0
//T: :s$data1$data2     R: !2\n
//Error on reading second data packet received error = 2
//R: :c\n               T: ;c\n
//Command end is received from microcontroller, GUI will stop records

struct RE_Settings
{
    uint16_t    food;
    uint32_t    press_interval;
    uint32_t    sound_length;
    uint8_t     manual;
    uint32_t    min_delay;
    uint32_t    max_delay;
};

#endif
