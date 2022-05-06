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
        // get one frame from camera to know frame size and type
        cap >> src;
        // check if we succeeded
        if (src.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            return -1;
        }
        bool isColor = (src.type() == CV_8UC3);
        //--- INITIALIZE VIDEOWRITER
        VideoWriter writer;
        int codec = VideoWriter::fourcc('M', 'P', '4', 'V');  // select desired codec (must be available at runtime)
        double fps = 25.0;                          // framerate of the created video stream
        string filename = "./live.mp4";             // name of the output video file
        writer.open(filename, codec, fps, src.size(), isColor);
        cap.set(CAP_PROP_BUFFERSIZE, 3);
        // check if we succeeded
        if (!writer.isOpened()) {
            cerr << "Could not open the output video file for write\n";
            return -1;
        }
        //--- GRAB AND WRITE LOOP
        for (;;)
        {
            // check if we succeeded
            if (!cap.read(src)) {
                cerr << "ERROR! blank frame grabbed\n";
                break;
            }
            // encode the frame into the videofile stream
            writer.write(src);
            // show live and wait for a key with timeout long enough to show images
            imshow("Live", src);
            if (waitKey(5) >= 0)
                break;
        }*/
    MainWindow w;
    w.show();
    return a.exec();
}
