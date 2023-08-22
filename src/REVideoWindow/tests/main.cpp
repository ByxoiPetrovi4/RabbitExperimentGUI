#include <iostream>
#include "videowindow.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    QSVideo::VideoParams params;
    params.api = cv::CAP_V4L2;
    memcpy(params.fourcc, "MJPG", 4);
    params.fheight = 720;
    params.fwidth = 1280;
    params.fps = 30;
    strcpy(params.src_name, "4");
    VideoWindow vw(params, "RE_Video2", "/tmp/video2.avi", 5.f);
    qDebug() << params.fwidth << params.fheight;
    vw.show();
    params.api = cv::CAP_V4L2;
    memcpy(params.fourcc, "MJPG", 4);
    params.fheight = 720;
    params.fwidth = 1280;
    params.fps = 30;
    strcpy(params.src_name, "0");
    VideoWindow vw1(params, "RE_Video0", "/tmp/video0.avi", 5.f);
    vw1.show();
    return a.exec();

}
