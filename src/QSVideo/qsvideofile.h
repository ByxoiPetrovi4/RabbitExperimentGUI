#ifndef QSVIDEOSOURCEF_H
#define QSVIDEOSOURCEF_H

#include <fstream>
#include "qsvideostream.h"

class QSVideoStaticFile : public QSVideoStream
{
    Q_OBJECT
public:
    enum BufferType : uint8_t
    {
        QSV_BUFFORWARD = 0x0,
        QSV_NOBUF,
        QSV_BUFBACK,
        QSV_BUFDUAL
    };
private:
    const BufferType    buf_type;
    QString             key;
    cv::VideoCapture    cap;
    qs::FrameT          current_frame;
    qs::TimeT           current_time;
    float               FPS = 24000000.f;
    std::vector<qs::TimeT>
                        timecodes;

    inline qs::TimeT getTimecode(qs::FrameT fr)
    {
        if(timecodes.empty())
            return fr/FPS*1000000.f;
        else
            return timecodes[fr];
    }
    inline bool isInRange(int32_t pos)
    {
        if(pos < 0)
            if((-pos) >= info.buffer_taillen)
                return false;
            else
                return true;
        else
            if(pos < info.buffer_lenw)
                return true;
            else
                return false;
    }
public:
    QSVideoStaticFile() = delete;
    QSVideoStaticFile(QString source, QString key, BufferType bt = QSV_BUFFORWARD,
                      uint8_t buffersz = 1, QObject *parent=nullptr);
    ~QSVideoStaticFile();

    void        getFrame(QSVideo::Frame& out, int32_t i) override;
    void        getFrame(QSVideo::Frame& out) override;

    UStatus     update() override;
};

#endif // QSVIDEOSOURCEF_H
