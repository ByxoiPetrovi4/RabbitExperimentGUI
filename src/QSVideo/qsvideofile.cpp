#include "qsvideofile.h"

QSVideoStaticFile::QSVideoStaticFile(QString source, QString key, BufferType bt,
                                     uint8_t buffersz, QObject *parent) : QSVideoStream{key, parent}, buf_type(bt), key{key}
{
    if(!cap.open(source.toStdString()))
        throw 1;
    cv::Mat tst;
    cap.read(tst);
    if(tst.empty())
        throw 1;
    auto directory = qs::getFileDirectory(source);
    std::fstream timeStampsStream;
    timeStampsStream.open((directory + "timestamps.csv").toStdString(), std::ios_base::in);
    if(timeStampsStream.is_open())
    {
        qint64 a;
        while(timeStampsStream >> a)
        {
            timecodes.push_back(a);
        }
        //TODO: add format reading from stream
    }
    cap.set(cv::CAP_PROP_POS_FRAMES, 0);
    current_frame = 0;
    current_time = 0;
    mem = new QSharedMemory();
    mem->setNativeKey(key);
    //Setup info about object
    info.size = {tst.cols, tst.rows};
    info.mat_memsize = (info.size.height*info.size.width)*
            sizeof(uchar)*formatToBytes(info.video_settings.cvtype);
    info.buffer_posr = 0;
    info.buffer_lenw = 0;
    info.buffer_size = buffersz;
    strncpy(info.key, key.toStdString().c_str(), 128);
    strncpy(info.source_key, key.toStdString().c_str(), 128);

    FPS = cap.get(cv::CAP_PROP_FPS);
    info.video_settings.fps = FPS;
    info.video_settings.fheight = tst.rows;
    info.video_settings.fwidth = tst.cols;
    ((int32_t*)info.video_settings.fourcc)[0] = cap.get(cv::CAP_PROP_FOURCC);
    info.video_settings.cvtype = tst.type();
    info.video_settings.all_frames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    info.video_settings.video_length = getTimecode(info.video_settings.all_frames-1);

    //Allocating memory for all significant data
    if(!mem->create(sizeof(QSVideo::Info) + info.buffer_size*info.mat_memsize +
                    sizeof(QSVideo::Frame)*info.buffer_size))
    {
        qDebug() << mem->errorString();
        delete mem;
        mem = new QSharedMemory();
        mem->setNativeKey(key);
        mem->attach();
        qDebug() << mem->errorString();
        mem->detach();
        qDebug() << mem->errorString();
        if(!mem->create(sizeof(QSVideo::Info) + info.buffer_size*info.mat_memsize +
                        sizeof(QSVideo::Frame)*info.buffer_size))
        {
            qDebug() << mem->errorString();
            throw 1;    //TODO: add proper exception
        }
    }

    info.state = QSV_READY;

    mlock();
    //ptr to the first segment of matrix data
    uchar *ptr = static_cast<uchar*>(mem->data()) + sizeof(QSVideo::Info) +
            sizeof(Frame)*info.buffer_size;
    //writing info about frame to shared memory
    QSVideo::Info *inf = static_cast<QSVideo::Info*>(mem->data());
    //initiliazation of buffer matrixes
    info.matsegment_start = sizeof(QSVideo::Info);
    Frame* frame_array = (Frame*)(static_cast<uchar*>(mem->data()) + info.matsegment_start);
    for(int i = 0; i < info.buffer_size; i++)
    {
        frame_array[i].mat = cv::Mat(info.size, info.video_settings.cvtype, ptr);
        ptr += info.mat_memsize;
    }
    *inf = info;
    munlock();
    emit ready();
}

QSVideoStaticFile::~QSVideoStaticFile()
{

}

void QSVideoStaticFile::getFrame(QSVideo::Frame& out, int32_t i)
{
    if(i < 0 && buf_type > QSV_NOBUF)
        throw 7;    //TODO: add proper exception could watch prev frame with this type of buffer
    mlock();
    info = *_getInfo();
    if(isInRange(i))
        out = *_getFrameC(i);
    else
        out = Frame();
    munlock();
}

void QSVideoStaticFile::getFrame(QSVideo::Frame& out)
{
    mlock();
    info = *_getInfo();
    if(info.buffer_lenw>0)
        out = *_getFrameC();
    else
        out = Frame();
    munlock();
}

QSVideo::UStatus QSVideoStaticFile::update()
{
    mlock();
    info = *_getInfo();
    switch (info.state) {
    case QSV_READY:
    case QSV_WARNING:
    {
        if(info.rewindFlag)
        {
            if(info.rewindFlag==QSV_TIMEREWIND)
                if(timecodes.empty())
                {
                    cap.set(cv::CAP_PROP_POS_MSEC, info.rewindTime <= 0 && info.rewindTime > info.video_settings.video_length ?
                                0 : info.rewindTime/1000);
                    info.rewindTime = 0;
                }
                else
                {
                    auto frame = std::lower_bound(timecodes.begin(), timecodes.end(),
                                                  info.rewindTime);
                    info.rewindTime = 0;
                    cap.set(cv::CAP_PROP_POS_FRAMES, std::distance(timecodes.begin(), frame)-1);
                }
            else if(info.rewindFlag==QSV_FRAMEREWIND)
            {
                cap.set(cv::CAP_PROP_POS_FRAMES, info.rewindFrame < 0 && info.rewindFrame > info.video_settings.all_frames ? 0 : info.rewindFrame);
                info.rewindFrame = 0;
            }
            info.rewindFlag = QSV_NOREWIND;
            info.buffer_posr = 0;
            info.buffer_lenw = 0;
            cursors_return();
            /*for(int i = 0; i < info.buffer_size; i++)
                _getFrame(i)->clear();
            for(auto children : qsfChildren)
            {
                children->clear();
            }*/
            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
            ///TODO: rewind for children
            /// TODO: add jump for frame to time vector
        }
        if(info.buffer_lenw >= info.buffer_size)
        {
            move();
            *_getInfo() = info;
            munlock();
            return QSV_AWAIT;
        }
        auto fr = _getFrameC(info.buffer_lenw);
        if(!cap.read(fr->mat))
        {
            info.state = QSV_STOP;
            *_getInfo() = info;
            munlock();
            return QSV_EXIT;
        }
        current_frame++;
        current_time = getTimecode(current_frame);
        fr->frame = current_frame;
        fr->time = current_time;
        //debug label on matrix
        auto font                   = cv::FONT_HERSHEY_SIMPLEX;
        auto bottomLeftCornerOfText = cv::Point2i(10,200);
        float fontScale             = 1.f;
        cv::Vec3b fontColor         = {255,255,0};
        float thickness             = 1.f;
        int lineType                = 2;

        double ms = fr->time/1000;
        cv::putText(fr->mat,std::to_string(ms),
            bottomLeftCornerOfText,
            font,
            fontScale,
            fontColor,
            thickness,
            lineType);
        info.buffer_lenw++;
        *_getInfo() = info;
        munlock();
        return QSV_NEW;
    }
    case QSV_STOP:
        munlock();
        return QSV_AWAIT;
    case QSV_NOTINITED:
        munlock();
        return QSV_EXIT;
    default:
        munlock();
        return QSV_EXIT;
    }
}
