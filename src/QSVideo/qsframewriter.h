#ifndef QSFRAMEWRITER_H
#define QSFRAMEWRITER_H

#include <fstream>
#include "qsvideo.h"

class QSFrameWriter
{
    cv::VideoWriter v_out;
    std::fstream    t_out;
public:
    QSFrameWriter(std::string filename, QSVideo::VideoParams params);
    void        write(const QSVideo::Frame &fr);
};

#endif // QSFRAMEWRITER_H
