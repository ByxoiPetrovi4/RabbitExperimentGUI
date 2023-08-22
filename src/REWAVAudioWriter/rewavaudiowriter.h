#ifndef REWAVAUDIOWRITER_H
#define REWAVAUDIOWRITER_H

#include <QDebug>
#include <inttypes.h>
#include <RtAudio.h>
#include <fstream>
#include <string>
#include <cstdio>

class REWavAudioWriter
{
public:
    struct  Params
    {
        int16_t     channelsNumber;
        int32_t     sampleRate;
        int16_t     bitsPerSample;
        char        file[256];
    };
    struct Header
    {
        int8_t      riff[4] = {'R', 'I', 'F', 'F'};
        int32_t     fileSize = 36;
        int8_t      wav[8] = {'W', 'A', 'V', 'E', 'f', 'm', 't', ' '};
        int32_t     format = 16;
        int16_t     type = 1;
        int16_t     channelsNumber;
        int32_t     sampleRate;
        int32_t     bitRate;
        int16_t     bytesChannelsPerSample;
        int16_t     bitsPerSample;
        int8_t      data[4] = {'d', 'a', 't', 'a'};
        int32_t     dataSize = 0;
    };
private:
    Header          header;
    std::fstream    output;
    RtAudio         *adc;
    bool            pause = false;


public:
    REWavAudioWriter(Params params);
    ~REWavAudioWriter();

    void write(char* inputBuf, uint32_t frames);
    static int record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
               double streamTime, RtAudioStreamStatus status, void* userData);
    static void start(REWavAudioWriter::Params params, std::shared_ptr<REWavAudioWriter> &ret);
public slots:
    void pausePlay();
    void stop();
};

#endif // REWAVAUDIOWRITER_H
