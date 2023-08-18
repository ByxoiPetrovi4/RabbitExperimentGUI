#ifndef QSVIDEOUSER_H
#define QSVIDEOUSER_H

#include "qsvideo.h"

class QSVideoUser : public QSVideo
{
private:
    Q_OBJECT
    int         user_id;
    bool        user_active = true;
public:
    QSVideoUser(QString key, QObject* parent = nullptr);

    ///Return status of user
    inline bool active(){return user_active;};
    ///Set user inactive update will skip this user
    void        stop();
    ///Set user active cursor will be set to actual 0
    void        resume();
    ///Copy data from actual cursor position to out
    /// move cursor by 1
    void        getFrame(QSVideo::Frame &out) override;
    ///Return last frame in queue
    void        getFrameL(QSVideo::Frame &out);
    ///Find closest frame to time but timecode for frame must be greater than time
    /// move cursor to found frame, if there is no frame with needed out will be empty
    void        getFrameT(QSVideo::Frame &out, qs::TimeT time) override;
    ///Vector length will be fill with frames from the actual of the user
    ///If set of data is moved forward by 1 frame
    ///than first element will be removed and new last added
    void        getFrameSet(std::vector<QSVideo::Frame> &set);
    ///Find closest frame in dataset with timecode more than time
    /// cursor will be moved to this frame
    void        getClosestFrame(QSVideo::Frame &out, qs::TimeT time);
};

#endif // QSVIDEOUSER_H
