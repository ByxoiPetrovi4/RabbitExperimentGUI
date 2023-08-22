#include "rewavaudiowriter.h"

REWavAudioWriter::REWavAudioWriter(REWavAudioWriter::Params params)
{
    header.channelsNumber = params.channelsNumber;
    header.sampleRate = params.sampleRate;
    header.bitsPerSample = params.bitsPerSample;
    header.bitRate = (header.sampleRate*header.bitsPerSample*header.channelsNumber)/8;
    header.bytesChannelsPerSample = (header.bitsPerSample*header.channelsNumber)/8;
    output.open(params.file, std::ios_base::out | std::ios_base::binary);
    if(!output.is_open())
        throw 1;    //TODO: add proper exception
    output.write((char*)&header, sizeof(Header));
}

REWavAudioWriter::~REWavAudioWriter()
{
    if(!output.is_open())
        return;
    output.seekp(output.beg);
    output.write((char*)&header, sizeof(Header));
    output.close();
    adc->stopStream();
    delete adc;
}

void REWavAudioWriter::write(char *inputBuf, uint32_t frames)
{
    if(pause)
        return;
    output.write(inputBuf, frames*header.bytesChannelsPerSample);
    header.dataSize += frames*header.bytesChannelsPerSample;
    header.fileSize = header.dataSize + 36;
}

int REWavAudioWriter::record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void* userData)
{
    if(status)
        qDebug() << "stream overflow detected!\n";
    REWavAudioWriter *writer;
    writer = static_cast<decltype(writer)>(userData);
    writer->write((char*)inputBuffer, nBufferFrames);
    return 0;
}

void REWavAudioWriter::start(REWavAudioWriter::Params params,
                             std::shared_ptr<REWavAudioWriter> &ret)
{
    RtAudio *adc;
    ret = std::make_shared<REWavAudioWriter>(params);
    try {
        adc = new RtAudio();
        if(adc->getDeviceCount() < 1)
        {
            qDebug() << "No audio devices found!";
            return;
        }
        RtAudio::StreamParameters parameters;
        parameters.deviceId = adc->getDefaultInputDevice();
        parameters.nChannels = 2;
        parameters.firstChannel = 0;
        uint32_t sampleRate = params.sampleRate;
        uint32_t bufferFrames = 256;
        ret->adc = adc;
        adc->openStream(NULL, &parameters, RTAUDIO_SINT16, sampleRate, &bufferFrames, &record, ret.get());
        adc->startStream();
    }  catch (RtAudioError &e) {
        e.printMessage();
        qDebug() << "Couldn't process sound!";
        return;
    }
    return;
}

void REWavAudioWriter::pausePlay()
{
    pause = !pause;
}

void REWavAudioWriter::stop()
{
    if(!adc->isStreamOpen())
        return;
    try {
        adc->stopStream();
    }  catch (RtAudioError &e) {
        e.printMessage();
    }
}
