#include "qsvideocamera.h"

QSVideoCamera::QSVideoCamera(QString key, VideoParams &params, int buffersz, QObject *parent)
    : QSVideoStream{key, parent}
{
    using namespace std::chrono_literals;
    info.type = QSV_CAMERA;
    std::vector<int> vec_params;
    vec_params.push_back(cv::CAP_PROP_FOURCC);
    vec_params.push_back(((int*)params.fourcc)[0]);
    vec_params.push_back(cv::CAP_PROP_FPS);
    vec_params.push_back(params.fps);
    vec_params.push_back(cv::CAP_PROP_FRAME_HEIGHT);
    vec_params.push_back(params.fheight);
    vec_params.push_back(cv::CAP_PROP_FRAME_WIDTH);
    vec_params.push_back(params.fwidth);
    std::string str(params.src_name);
    try
    {
        int i = std::stoi(str);
        if(!cap.open(i, params.api, vec_params))
            throw 1;    //TODO: add proper exception
    }
    catch(...)  //if(stoi doesn't convert)
    {
        if(!cap.open(params.src_name, params.api, vec_params))
            throw 1;    //TODO: add proper exception
    }
    int32_t fourcc = cap.get(cv::CAP_PROP_FOURCC);
    memcpy(params.fourcc, &fourcc, 4);
    params.fps = cap.get(cv::CAP_PROP_FPS);
    params.fheight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    params.fwidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    cv::Mat tst;
    qDebug() << cap.get(cv::CAP_PROP_FPS);;
    cap.read(tst);
    if(tst.empty())
        throw 1;    //TODO: add proper exception
    mem = new QSharedMemory();
    mem->setNativeKey(key);
    //Setup info about object
    info.size = {tst.cols, tst.rows};
    info.mat_memsize = (info.size.height*info.size.width)*
            sizeof(uchar)*formatToBytes(info.video_settings.cvtype);
    info.buffer_posr = 0;
    info.buffer_lenw = 0;
    info.buffer_size = buffersz;
    info.video_settings = params;
    strncpy(info.key, key.toStdString().c_str(), 128);
    strncpy(info.source_key, key.toStdString().c_str(), 128);

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
    //start = std::chrono::high_resolution_clock::now().time_since_epoch();
    mlock();
    //ptr to the first segment of matrix data
    uchar *ptr = static_cast<uchar*>(mem->data()) + sizeof(QSVideo::Info) +
            sizeof(Frame)*info.buffer_size;
    //initiliazation of buffer matrixes
    info.matsegment_start = sizeof(QSVideo::Info);
    Frame* frame_array = (Frame*)(static_cast<uchar*>(mem->data()) + info.matsegment_start);
    for(int i = 0; i < info.buffer_size; i++)
    {
        frame_array[i].mat = cv::Mat(info.size, info.video_settings.cvtype, ptr);
        ptr += info.mat_memsize;
    }
    *_getInfo() = info;
    munlock();
    start = std::chrono::high_resolution_clock::now();
    emit ready();
    nextRead = start + 1000ms/((long long)info.video_settings.fps);
    cap.grab();
}

QSVideo::UStatus QSVideoCamera::update()
{
    using namespace std::chrono_literals;
    mlock();
    info = *_getInfo();
    switch (info.state) {
    case QSV_READY:
    case QSV_WARNING:
    {
        if(info.buffer_lenw >= info.buffer_size)
        {
            if(!move())
            {
                *_getInfo() = info;
                munlock();
                return QSV_AWAIT;
            }
        }
        auto fr = _getFrameC(info.buffer_lenw);
        if(!cap.retrieve(fr->mat))
        {
            info.state = QSV_STOP;
            *_getInfo() = info;
            munlock();
            return QSV_EXIT;
        }
        auto dur =
                std::chrono::high_resolution_clock::now() - start;
        auto durus = std::chrono::duration_cast<std::chrono::microseconds>(dur);
        current_time = durus.count();
        current_frame++;
        fr->frame = current_frame;
        fr->time = current_time;/*
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
            lineType);*/
        info.buffer_lenw++;
        *_getInfo() = info;
        munlock();
        cap.grab();
        auto now = std::chrono::high_resolution_clock::now();
        if(now < nextRead - 1ms)
        {
            auto sleep = nextRead - now - 1ms;
            nextRead += 1000ms/((long long)info.video_settings.fps);
            std::this_thread::sleep_for(sleep);
        }
        else
        {
            nextRead = now + 1000ms/((long long)info.video_settings.fps);
            std::this_thread::sleep_for(1ms);
        }
        return QSV_NEW;
    }
    case QSV_STOP:
        munlock();
        return QSV_AWAIT;
    case QSV_NOTINITED:
        munlock();
        return QSV_EXIT;
    case QSV_KILL:
        munlock();
        return QSV_EXIT;
        break;
    default:
        munlock();
        return QSV_EXIT;
    }
}