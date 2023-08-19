#ifndef QSVIDEO_H
#define QSVIDEO_H

#include <QSharedMemory>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include "include/qsheader.h"
#include "src/QSTransform/qstransform.h"

namespace qs {
    static struct
    {
        char    key[128] = {0};
        int     users = 0;
        QMutex  mtx;
    }   VideosMutexes[QSVideoMax];

    static int      VideosMutexCounter = 0;
}

///Class for work with different videosources
class QSVideo : public QObject
{
    Q_OBJECT
public:
    ///Type of video
    enum SourceType : uint8_t
    {
        QSV_USER = 0x0,
        QSV_STATICFILE,
        QSV_CAMERA,
        QSV_GENERATED
    };

    ///State for video stream processing
    enum State : uint8_t
    {
        QSV_READY = 0x0,
        QSV_WARNING,
        QSV_NOTINITED,
        QSV_STOP,
        QSV_ERROR,
        QSV_KILL
    };

    ///Update status
    enum UStatus : uint8_t
    {
        QSV_NEW = 0x0,
        QSV_AWAIT,
        QSV_SKIP,
        QSV_EXIT
    };

    enum CopyResult : uint8_t
    {
        QSV_OK = 0x0,
        QSV_MATRIXSIZE,
        QSV_DATATYPE,
        QSV_EMPTY,
        QSV_BUFFER
    };

    enum Rewind : uint8_t
    {
        QSV_NOREWIND = 0x0,
        QSV_TIMEREWIND,
        QSV_FRAMEREWIND
    };

    static const uint16_t MAX_HANDLERS = 16;

    ///USED in frames with source type
    struct VideoParams
    {
        char        src_name[256] = {0};
        int         api = cv::CAP_ANY;
        char        fourcc[4] = {'M', 'J', 'P', 'G'};
        int         cvtype = CV_8UC3;
        qs::FrameT  all_frames = 0;
        qs::TimeT   video_length = 0;
        float       fps = 0;
        int         fheight = 0;
        int         fwidth = 0;
    };

    static const VideoParams DefaultVideoParams;

    //store data about frame
    struct Info
    {
        //Video source type
        //In shared memory segment type status is always != QSV_USER
        //only in local copy of videostream in stack memory type set to USER
        SourceType      type = QSV_STATICFILE;
        //Is active
        State           state = QSV_NOTINITED;
        //Rewind flag
        Rewind          rewindFlag = QSV_NOREWIND;
        //Rewind timing
        qs::TimeT       rewindTime = 0;
        //Rewind timing for frame
        //WARNING! if rewind is doing via frame than rewindTime should be set to 0
        qs::FrameT      rewindFrame = 0;
        //size of matrix
        cv::Size        size;
        //memory size for 1 matrix
        size_t          mat_memsize;
        //start of cv::mat memory segment
        size_t          matsegment_start;
        //position of reader
        int32_t         buffer_posr;
        //length of written elements
        int32_t         buffer_lenw;
        //position of tail. FOR dual or back side buffer
        int32_t         buffer_taillen = 0;
        //number of matrixes allocated in memory
        int32_t         buffer_size;
        //transform type for transform used in frame
        QSTransform::TransformType
                        trans_type = QSTransform::TRANSFORM_STATIC;
        //Table of iterators for users
        int16_t         users_iters[MAX_HANDLERS] = {-1};
        //Cureent users
        uint16_t        users_all = 0;
        //number of active users_active
        uint16_t        users_active = 0;
        //Key for memory segment
        char            key[128];
        //Key for source segment
        char            source_key[128];
        //Key for parent segment
        char            parent_key[128];
        VideoParams     video_settings;
    };

    class Frame
    {
    private:
        bool            _empty = true;
    public:
        qs::FrameT      frame;
        qs::TimeT       time;

        QSTransform::TransformType
                        transform_status;
        cv::Rect2i      transform_rect;

        cv::Mat         mat;

        inline Frame    clone()
        {
            Frame ret;
            ret._empty = false;
            ret.frame = frame;
            ret.time = time;
            ret.mat = mat.clone();
            return ret;
        }
        Frame();
        Frame(qs::FrameT _frame, qs::TimeT _time, cv::Mat _mat);
        Frame(const Frame&);
        ~Frame();

        bool            empty() const;
        void            set_transformrect(cv::Rect2i);

        friend QSVideo;
        friend class QSVideoStaticFile;
        friend class QSVideoUser;
    };

protected:
    //local copy of type if it's user copy it's set to QSV_USER
    SourceType      type        = QSV_STATICFILE;
    //info in this class is local copy of shared memory info
    Info            info;
    //Index for mutex
    int             mutex_index;
    //pointer to qsvideo which provide data
    QSVideo         *source     = nullptr;
    //pointer to parent of this video source
    QSVideo         *parent     = nullptr;
    //transform pointer to method which make this video from parent
    QSTransform     *trans      = nullptr;
    //pointer to shared memory segment
    QSharedMemory   *mem;

    inline Info *_getInfo()
    {
        return static_cast<QSVideo::Info*>(mem->data());
    }
    ///i is direct position in array
    inline QSVideo::Frame  *_getFrameD(uint32_t i = 0)
    {
        uchar *ptr = static_cast<uchar*>(mem->data());
        ptr += info.matsegment_start;
        auto arr = static_cast<QSVideo::Frame*>((void*)ptr);
        return &arr[i];
    }
    ///i is position in circle queue
    inline QSVideo::Frame  *_getFrameC(int32_t i = 0)
    {
        uchar *ptr = static_cast<uchar*>(mem->data());
        ptr += info.matsegment_start;
        auto arr = static_cast<QSVideo::Frame*>((void*)ptr);
        return &arr[(i+info.buffer_posr)%info.buffer_size];
    }
    ///element is position in array of matrixes
    inline void     *getMatDataPtr(int32_t element)
    {
        return static_cast<uchar*>(mem->data()) +
               info.matsegment_start + info.buffer_size*sizeof(Frame) +
               element*info.mat_memsize;
    }
    ///Init frame from parent copy of frame
    inline void     initFrame(QSVideo::Frame &f, QSVideo::Frame &pf)
    {
        f.frame = pf.frame;
        f.time = pf.time;
    }
    ///Convert cv data format to size in bytes
    inline size_t   formatToBytes(int data_format)
    {
        return (1+data_format/8);
    }
    ///Memory lock
    inline void mlock() const
    {
        mem->lock();
        qs::VideosMutexes[mutex_index].mtx.lock();
    }
    ///Memory unlock
    inline void munlock() const
    {
        mem->unlock();
        qs::VideosMutexes[mutex_index].mtx.unlock();
    }
    ///Memory lock
    inline bool mlock(int ms) const
    {
        mem->lock();
        if(qs::VideosMutexes[mutex_index].mtx.tryLock(ms))
            return true;
        else
        {
            mem->unlock();
            return false;
        }
    }
    ///Check is it possible to move cursor for all users
    inline int move()
    {
        if(info.state > QSV_WARNING)
            return 0;
        int mx = info.buffer_lenw;
        if(info.users_all)
        {
            for(int i = 0; i < info.users_all; i++)
            {
                if(info.users_iters[i]!=-1)
                    if(info.users_iters[i]<mx)
                        mx = info.users_iters[i];
            }
            if(mx==0)
                return 0;
        }
        info.buffer_lenw -= mx;
        info.buffer_posr = info.buffer_posr - mx > 0 ? info.buffer_posr-mx : 0;
        for(int i = 0; i < info.users_all; i++)
        {
            info.users_iters[i] = info.users_iters[i] != -1 ? info.users_iters[i]-mx : -1;
        }
        return mx;
    }
    ///Set cursor for all active users to 0
    inline void cursors_return()
    {
        for(int i = 0; i < info.users_all; i++)
        {
            info.users_iters[i] = info.users_iters[i] < 0 ? -1 : 0;
        }
    }
    ///Return is sourcetype could be rewinded
    static inline bool isTypeRewindable(SourceType t)
    {
        switch(t)
        {
        case QSV_CAMERA:
        case QSV_GENERATED:
            return false;
        default:
            return true;
        }
    }
    //Default constructor set video status as not inited
    QSVideo(QString key, QObject* parent=nullptr);
signals:
    void                    ready();
    void                    warning(int code);
    void                    error(int code);
    void                    end();

public:
    //Use only for already existing segments
    //QSVideo(QString &&memoryKey, QSVideo::SourceType srcType = QSV_USER,
    //        QSVideo::VideoParams videoParams = DefaultVideoParams);
    //Default destructor
    virtual ~QSVideo();

    ///Return state for this video
    State getState();
    ///Return type of this video
    SourceType getType() const;
    /// Get key for memory segment
    const QString getKey() const;
    ///Get parents key
    const QString getSourceKey() const;
    ///Return number of frames in video or return 0 if its videostream
    inline qs::FrameT getNumberOfFrames() const {return info.video_settings.all_frames;}
    ///Return length of video in us or return 0 if its videostream
    inline qs::TimeT getVideoLength() const {return info.video_settings.video_length;}
    ///IN ALL FUNCTIONS BELOW
    /// IF out is aneccesiable videostream should return empty frame

    ///Return frame from buffer with i as index in the buffer
    virtual void        getFrame(QSVideo::Frame& out, int32_t i);
    ///Return first frame from queue or the first analog
    virtual void        getFrame(QSVideo::Frame& out);
    ///Return frame closest to time but frame timecode must be greater than time
    virtual void        getFrameT(QSVideo::Frame& out, qs::TimeT time);
    ///Set flags for rewind time and other stuff
    /// if rewind to this time isn't possible throw out of range
    /// or if rewind isn't supported throw rewind isn't possible
    virtual void        rewindT(qs::TimeT);
    ///Set flags for rewind frame and other stuff
    /// if rewind to this frame isn't possible throw out of range
    /// or if rewind isn't supported throw rewind isn't possible
    virtual void        rewindF(qs::FrameT);

    static void         videoParamsToVector(const VideoParams &params,
                                            std::vector<int> &vec_params);
public slots:
    ///Function realease all related videousers and source
    /// will kill qsvideo even in another process
    void kill();
};

#endif // QSVIDEO_H
