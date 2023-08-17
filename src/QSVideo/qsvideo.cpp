#include "qsvideo.h"

QSVideo::Frame::Frame()
{
    _empty = true;
    frame = 0;
    time = 0;
    mat = cv::Mat();
}

QSVideo::Frame::Frame(qs::FrameT _frame, qs::TimeT _time, cv::Mat _mat)
{
    _empty = false;
    frame = _frame;
    time = _time;
    mat = _mat.clone();
}

QSVideo::Frame::Frame(const Frame& cpy) : _empty(cpy._empty), frame(cpy.frame), time(cpy.time),
    transform_status(cpy.transform_status), transform_rect(cpy.transform_rect)
{
    if(mat.data==nullptr || mat.total() < cpy.mat.total())
        mat = cpy.mat.clone();
    else
    {
        cpy.mat.copyTo(mat);
    }
}

QSVideo::Frame::~Frame()
{
    //delete mat;
}

bool QSVideo::Frame::empty() const
{
    return _empty;
}

void QSVideo::Frame::set_transformrect(cv::Rect2i rect)
{
    transform_rect = rect;
    transform_status = transform_status | QSTransform::TRANSFORM_READY;
}


QSVideo::QSVideo(QString key, QObject *parent) : QObject{parent}
{
    if(key.toStdString().length()>127)
    {
        throw 1;    //TODO: add proper exception
    }
    info.state = QSV_NOTINITED;
    for(int i = 0; i < qs::QSVideoMax; i++)
    {
        if(!strcmp(qs::VideosMutexes[i].key, key.toStdString().c_str()))
        {
            qs::VideosMutexes[i].users++;
            mutex_index = i;
            return;
        }
    }
    for(int i = 0; i < qs::QSVideoMax; i++)
    {
        if(qs::VideosMutexes[i].users==0)
        {
            qs::VideosMutexes[i].users++;
            mutex_index = i;
            strncpy(qs::VideosMutexes[i].key, key.toStdString().c_str(), 128);
            return;
        }
    }
    throw 1;    //TODO: add proper exception
}

/*QSVideo::QSVideo(QString &&memoryKey, QSVideo::SourceType srcType,
                 QSVideo::VideoParams videoParams)
{
    switch (srcType) {
    case QSV_USER:
    {
        mem = new QSharedMemory();
        mem->setNativeKey(memoryKey);
        if(!mem->isAttached())
            if(!mem->attach(QSharedMemory::ReadWrite))
                throw 1; //TODO: add proper exception
        mem->lock();
        const QSVideo::Info *inf = (QSVideo::Info*)mem->constData();
        info = *inf;
        mem->unlock();
    }
        break;
    case QSV_CAMERA:
        mem = new QSharedMemory();
        mem->setNativeKey(memoryKey);

        break;
    case QSV_GENERATED:
        break;
    case QSV_STATICFILE:
        break;
    default: throw 1;   //TODO: incorrect enum input
    }
}*/

QSVideo::~QSVideo()
{
    kill();
    if(info.state == QSV_NOTINITED)
        return;
    else
    {
        if(trans)
            delete trans;
        if(mem)
            delete mem;
    }
}

QSVideo::State QSVideo::getState()
{
    if(!mem)
        return QSV_NOTINITED;
    else
    {
        mlock();
        info = *_getInfo();
        munlock();
        return info.state;
    }
}

const QString QSVideo::getKey() const
{
    return info.key;
}

QSVideo::SourceType QSVideo::getType() const
{
    return type;
}

const QString QSVideo::getSourceKey() const
{
    return info.source_key;
}

void QSVideo::kill()
{
    mlock();
    _getInfo()->state = QSV_KILL;
    munlock();
    emit end();
}

void QSVideo::getFrame(QSVideo::Frame &out, int32_t i)
{
    if(info.buffer_size<=fabs(i))
    {
        out = QSVideo::Frame();
        throw 1;    //TODO: add proper exception
    }
    mlock();
    info = *_getInfo();
    if(info.buffer_lenw < fabs(i))
    {
        munlock();
        out = Frame();
        throw 1;    //TODO: add proper exception
    }
    out = *_getFrameC(i);
    munlock();
}

void QSVideo::getFrame(QSVideo::Frame &out)
{
    mlock();
    info = *_getInfo();
    if(info.buffer_lenw == 0)
    {
        munlock();
        out = Frame();
        return;
        //throw 1;    //TODO: add proper exception
    }
    out = *_getFrameC(info.buffer_lenw);
    munlock();
}

void QSVideo::getFrameT(QSVideo::Frame &out, qs::TimeT time)
{
    mlock();
    info = *_getInfo();
    qs::TimeT dif = 0;
    int nindex = 0;
    for(int i = 0; i < info.buffer_lenw; i++)
    {
        qs::TimeT ndif = time-_getFrameC(i)->time;
        if(ndif < dif)
        {
            dif = ndif;
            nindex = i;
        }
    }
    if(dif == 0)
        out = Frame();
    else
    {
        out = *_getFrameC(nindex);
    }
    munlock();
}

void QSVideo::rewindF(qs::FrameT f)
{
    if(source)
        return source->rewindF(f);
    if(!isTypeRewindable(type))
        throw 1;    //TODO: add proper exception noitrewindabletype!
    mlock();
    info = *_getInfo();
    info.rewindFlag = QSV_FRAMEREWIND;
    info.rewindFrame = f;
    *_getInfo() = info;
    munlock();
}

void QSVideo::rewindT(qs::TimeT t)
{
    if(source)
        return source->rewindT(t);
    if(!isTypeRewindable(type))
        throw 1;    //TODO: add proper exception noitrewindabletype!
    mlock();
    info = *_getInfo();
    info.rewindFlag = QSV_FRAMEREWIND;
    info.rewindFrame = t;
    *_getInfo() = info;
    munlock();
}

void QSVideo::videoParamsToVector(const VideoParams &params,
                                        std::vector<int> &vec_params)
{
    vec_params.clear();
    vec_params.push_back(cv::CAP_PROP_FOURCC);
    vec_params.push_back(((int*)params.fourcc)[0]);
    vec_params.push_back(cv::CAP_PROP_FPS);
    vec_params.push_back(params.fps);
    vec_params.push_back(cv::CAP_PROP_FRAME_HEIGHT);
    vec_params.push_back(params.fheight);
    vec_params.push_back(cv::CAP_PROP_FRAME_WIDTH);
    vec_params.push_back(params.fwidth);
}
