#ifndef RE_PROTOCOL
#define RE_PROTOCOL
//#define _CONTROLLER_
#define _MASTER_

#ifdef _CONTROLLER_
#include <stdint.h>
#include <string.h>
#include <math.h>
#endif

#ifdef _MASTER_
//#include "setup_header.h"
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <ctime>
#endif

struct RE_Settings
{
    int    food;
    int    press_interval;
    int    sound_length;
    int    manual;
    int    min_delay;
    int    max_delay;
};

#define ER_START_COMMUNICATION '~'
//Communications starts when after 3 start symbols
//master will reciece 3 start symbols as answer


//Possible keywords for protocol
//used in answers and commands
enum Keywords : char
{
    KSettings    = 's',
    KBeep        = 'b',
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
//symbols use after Event '&' as indicator of event
{
    EBeep       = 'b',
    EDrop       = 'd',
    EPress      = 'x',
    EIncPress   = 'i',
    ENoPress    = 'n',
    ETimeout    = 'y',
    ECorrect    = 'z'
};

enum SSymbols : char
//this symbols is reserved for protocol
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

#ifdef _CONTROLLER_
//eEvent use in controller state machine:
// ENothing = no event, EPause | EAnswer = pause answer to master required
// ESound = sound event "&b\n" send to master
// ESound | EAnswer = command reseved from master answer required ";b\n"
typedef enum : char
{
  CENothing,
  CEConnected,
  CEStart,
  CEManualTraining,
  CEAutoTraining,
  CEPedal,
  CEDrop,
  CEFeed,
  CEPause,
  CESound,
  CEEnd,
  CEAnswer = 0x40
} eEvent;
#endif

#endif
