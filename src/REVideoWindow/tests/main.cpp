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
    VideoWindow vw(params, "RE_Video2");
    qDebug() << params.fwidth << params.fheight;
    vw.show();
    return a.exec();

}
