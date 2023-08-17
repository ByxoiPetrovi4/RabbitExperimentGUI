#include <iostream>
#include <QString>
#include <time.h>
#include "qapplication.h"

#include "qsvideofile.h"
#include "qsvideocamera.h"
#include "qsvideouser.h"

int main(int argc, char** argv)
{
    /*cv::VideoCapture vc(4, cv::CAP_V4L2);
    vc.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    vc.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    vc.set(cv::CAP_PROP_FPS, 60);
    vc.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
    char fc[5] = {0};
    int a;
    a = vc.get(cv::CAP_PROP_FOURCC);
    memcpy(fc, &a, 4);
    std::cout << fc << '\n';
    while(1)
    {
        cv::Mat img;
        vc.read(img);
        cv::imshow("Aboba", img);
        int key = cv::waitKey(10);
        if(key == 27)
        {
            qDebug() << "Aboba";
            break;
        }
    }*/
        USE_CHLIT
    try{
                std::thread b([]()
                {
                    USE_CHLIT
                    QSVideo::VideoParams params;
                    params.api = cv::CAP_V4L2;
                    memcpy(params.fourcc, "MJPG", 4);
                    params.fheight = 720;
                    params.fwidth = 1280;
                    params.fps = 30;
                    strcpy(params.src_name, "/dev/video2");
                    QSVideoCamera vf("Aboba81", params);
                    while(vf.update()!=QSVideo::QSV_EXIT)
                    {
                    }
                });
                b.detach();
                std::this_thread::sleep_for(1000ms);
        std::thread a([]()
        {
            USE_CHLIT
            QSVideo::VideoParams params;
            params.api = cv::CAP_V4L2;
            memcpy(params.fourcc, "YUYV", 4);
            params.fheight = 640;
            params.fwidth = 480;
            params.fps = 30;
            strcpy(params.src_name, "/dev/video4");
            QSVideoCamera vf("Aboba8", params);
            while(vf.update()!=QSVideo::QSV_EXIT)
            {
            }
        });
        a.detach();
        std::this_thread::sleep_for(6000ms);
        QSVideoUser us("Aboba8");
        QSVideoUser us1("Aboba81");
        QSVideo::Frame out, out1;
        while(1)
        {
                us.getFrame(out);
                if(!out.empty())
                    if(!out.mat.empty())
                        cv::imshow("Aboba", out.mat);
                us1.getFrame(out1);
                if(!out1.empty())
                    cv::imshow("Aboba1", out1.mat);
                int key = cv::waitKey(1);
                if(key == 27)
                {
                    qDebug() << "Aboba";
                    us.kill();
                    us1.kill();
                    break;
                }
        }
    }  catch (...) {
        return 1;
    }
    std::this_thread::sleep_for(1000ms);
    return 0x0;

}
