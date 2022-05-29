#include "wrcamera.h"

using namespace cv;

CameraWriter::CameraWriter()
{

}

CameraWriter::CameraWriter(CameraSettings st) : CameraWriter()
{
    cmSettings = st;
}

CameraWriter::re_time CameraWriter::GetTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

CameraWriter::~CameraWriter()
{
}

void CameraWriter::write(const bool &flag)
{
    if(flag == writing)return;
    if(!writing && flag)
    {
        allframes += wframes;
        wframes = 0;
        writing = flag;
        writing_start = GetTimeStamp();
        return;
    }
    writing = flag;
    writing_stop += GetTimeStamp() - writing_start;
}

void CameraWriter::display(const bool &flag)
{
    displaying = flag;
}

void CameraWriter::stop()
{
    end = true;
}

bool CameraWriter::isWriting()
{
    return writing;
}

void CameraWriter::run()
{
    Mat src;
    frameOut.open("video_log.txt", std::ios::out | std::ios::app);
    VideoCapture cap(2, CAP_V4L2);
    writing = false;
    displaying = false;
    allframes = 0;
    end = false;
    workingDir[0] = '\0';
    writing_stop = 0;
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return ;
    }
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)
    cap.set(CAP_PROP_FOURCC, codec);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(CAP_PROP_FPS, 60.0);
    cap.set(CAP_PROP_AUTOFOCUS, true);
    int ccodec = cap.get(CAP_PROP_FOURCC);
    // get one frame from camera to know frame size and type
    cap >> src;
    //qDebug() << ccodec;
    // check if we succeeded
    if (src.empty()) {
        std::cerr << "ERROR! blank frame grabbed\n";
        return ;
    }
    bool isColor = (src.type() == CV_8UC3);
    //--- INITIALIZE VIDEOWRITER
    VideoWriter writer;
    double fps = 40.0;                          // framerate of the created video stream
    std::string filename = "./live.avi";             // name of the output video file
    writer.open(filename, codec, fps, src.size(), isColor);
    cap.set(CAP_PROP_BUFFERSIZE, 3);
    // check if we succeeded
    if (!writer.isOpened()) {
        std::cerr << "Could not open the output video file for write\n";
        return ;
    }
    re_time ls=GetTimeStamp();
    re_time ct=ls;
    re_time frame_time = 1000000 / fps;
    uint16_t _fps = 0;
    uint64_t writed_frames = 0;
    //--- GRAB AND WRITE LOOP
    for (;!end;)
    {
        // check if we succeeded
        if (!cap.read(src)) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        _fps++;
        ct=GetTimeStamp();
        if(ct-ls>1000000)
        {
            emit fpsRate(tr("FPS = ") + QString::number(_fps));
            _fps = 0;
            ls+=1000000;
        }
        // encode the frame into the videofile stream
        if(writing)
        {
            if(ct-writing_start > wframes*frame_time)
            {
                wframes++;
                writer.write(src);
                frameOut << allframes+wframes << "; " << writing_stop + ct - writing_start
                         << "\n";
            }
        }
        QImage qOriginalImage((uchar*)src.data, src.cols,
            src.rows, src.step, QImage::Format_RGB888);

        if(displaying) emit newFrame(QPixmap::fromImage(qOriginalImage));
    }
    writer.release();
    frameOut.close();
}
