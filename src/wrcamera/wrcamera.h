#ifndef _CAMERA_WRITER_
#define _CAMERA_WRITER_

#include <QThread>
#include <QImage>
#include <QPixmap>
#include <cstring>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <sys/time.h>

class CameraWriter : public QThread
{
    Q_OBJECT
public:
    struct CameraSettings
    {
       int id;
       double fps;
       double tgfps;
       double width;
       double height;
       int codec;
    };
    typedef uint64_t re_time;
    static re_time GetTimeStamp();
private:
    std::ofstream frameOut;
    CameraSettings cmSettings;
    bool end;
    bool writing;
    re_time writing_start;
    re_time writing_stop;
    uint64_t wframes;
    uint64_t allframes;
    bool displaying;
    char workingDir[128];

    void run() override;
signals:
    void newFrame(const QImage&);
    void fpsRate(const QString&);
public slots:
    void write(const bool&);
    void display(const bool&);
    void stop();
public:
    CameraWriter();
    CameraWriter(CameraSettings);
    bool isWriting();
    ~CameraWriter();
};

#endif
