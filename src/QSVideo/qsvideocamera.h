#ifndef QSVIDEOCAMERA_H
#define QSVIDEOCAMERA_H

#include "qsvideostream.h"

class QSVideoCamera : public QSVideoStream
{
    Q_OBJECT
    cv::VideoCapture cap;
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        start;
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        fps_time;
    qs::FrameT          current_frame = 0;
    qs::TimeT           current_time;
    float               FPS = 24000000.f;
    std::vector<qs::TimeT>
                        timecodes;
    uint32_t            fps_count = 0;
public:
    QSVideoCamera(QString key, VideoParams &params, int buffersz = 32,
                  QObject *parent=nullptr);
    auto inline     getStartTime(){return start;}
    UStatus         update() override;
};

#endif // QSVIDEOCAMERA_H
