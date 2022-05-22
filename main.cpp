#include "mainwindow.h"

#include <QApplication>

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*Mat src;
        // use default camera as video source
        VideoCapture cap(0, CAP_DSHOW);
        // check if we succeeded
        if (!cap.isOpened()) {
            cerr << "ERROR! Unable to open camera\n";
            return -1;
        }
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)
        cap.set(CAP_PROP_FOURCC, codec);
        cap.set(CAP_PROP_FRAME_WIDTH, 1920);
        cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
        qDebug() << int(cap.get(CAP_PROP_FPS));
        cap.set(CAP_PROP_FPS, 30.0);
        cap.set(CAP_PROP_AUTOFOCUS, true);
        int ccodec = cap.get(CAP_PROP_FOURCC);
        // get one frame from camera to know frame size and type
        cap >> src;
        qDebug() << ccodec;
        // check if we succeeded
        if (src.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            return -1;
        }
        bool isColor = (src.type() == CV_8UC3);
        //--- INITIALIZE VIDEOWRITER
        VideoWriter writer;
        double fps = 22.0;                          // framerate of the created video stream
        string filename = "./live.avi";             // name of the output video file
        writer.open(filename, codec, fps, src.size(), isColor);
        cap.set(CAP_PROP_BUFFERSIZE, 128);
        // check if we succeeded
        if (!writer.isOpened()) {
            cerr << "Could not open the output video file for write\n";
            return -1;
        }
        re_time ls=GetTimeStamp();
        uint16_t _fps = 0;
        //--- GRAB AND WRITE LOOP
        for (;;)
        {
            // check if we succeeded
            if (!cap.read(src)) {
                cerr << "ERROR! blank frame grabbed\n";
                break;
            }
            _fps++;
            if(GetTimeStamp()-ls>1000000)
            {
                qDebug() << _fps;
                _fps = 0;
                ls+=1000000;
            }
            // encode the frame into the videofile stream
            writer.write(src);
            // show live and wait for a key with timeout long enough to show images
            imshow("Live", src);
            if (waitKey(1) >= 0)
                break;
        }*/
    MainWindow w;
    w.show();
    return a.exec();
}
