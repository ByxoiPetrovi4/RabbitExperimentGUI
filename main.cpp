#include "mainwindow.h"

#include <QApplication>

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    namedWindow( "Webcam", WINDOW_AUTOSIZE );

        namedWindow( "Log_Polar", WINDOW_AUTOSIZE );
        VideoCapture capture;
        capture.open(0);
        double fps = capture.get( CAP_PROP_FPS );
        Size size(
            (int)capture.get( CAP_PROP_FRAME_WIDTH ),
            (int)capture.get( CAP_PROP_FRAME_HEIGHT )
        );
        VideoWriter writer;
        writer.open( "output.avi", VideoWriter::fourcc('M','J','P','G'), fps, size );
        Mat logpolar_frame(size, CV_8UC4), bgr_frame;
        for(;;) {
            capture >> bgr_frame;
            if( bgr_frame.empty() ) break; // end if done
            imshow( "Webcam", bgr_frame );
            writer << bgr_frame;
            char c = cv::waitKey(5);
            if( c == 27 ) break; // allow the user to break out
        }
        capture.release();
        writer.release();
    MainWindow w;
    w.show();
    return a.exec();
}
