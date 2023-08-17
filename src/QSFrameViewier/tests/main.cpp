#include <QApplication>
#include <QThread>
#include "qsframeviewier.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    auto f = std::thread([]()->void{
        //QSVideoStaticFile input("/home/roger/out.mkv", "423abu18");
        QSVideo::VideoParams params;
        params.api = cv::CAP_V4L2;
        memcpy(params.fourcc, "YUYV", 4);
        params.fheight = 720;
        params.fwidth = 1280;
        params.fps = 30;
        strcpy(params.src_name, "/dev/video2");
        QSVideoCamera input("423abu18", params);
        while(1)
        {
            input.update();
        }});
    f.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto g = std::thread([]()->void{
        //QSVideoStaticFile input("/home/roger/out.mkv", "423abu18");
        QSVideo::VideoParams params;
        params.api = cv::CAP_V4L2;
        memcpy(params.fourcc, "MJPG", 4);
        params.fheight = 720;
        params.fwidth = 1280;
        params.fps = 30;
        strcpy(params.src_name, "/dev/video0");

        cv::VideoWriter vw("/tmp/tst2.avi", ((int*)&params.fourcc)[0], 10, {params.fwidth, params.fheight});
        QSVideoCamera input("423abu19", params);
        QSVideo::Frame fr;
        fr.time = 0;
        while(1)
        {
            auto upd = input.update();
            if(upd == QSVideo::QSV_NEW)
            {
                input.getFrameT(fr, fr.time + 100000);
                vw.write(fr.mat);
            }
        }});
    g.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    QSFrameViewier gv(nullptr);
    gv.setFixedHeight(600);
    gv.setFixedWidth(800);
    gv.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    gv.setFrame("423abu19");
    gv.show();
    gv.play();
    QSFrameViewier fv(nullptr);
    fv.setFixedHeight(600);
    fv.setFixedWidth(800);
    fv.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    fv.setFrame("423abu18");
    fv.show();
    fv.play();
    return a.exec();
}
