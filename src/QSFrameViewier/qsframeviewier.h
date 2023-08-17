#ifndef QSFRAMEVIEWIER_H
#define QSFRAMEVIEWIER_H

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QStyle>
#include <QStyleOption>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <qthread.h>
#include <opencv2/opencv.hpp>
#include "src/QSVideo/qsvideouser.h"
#include "src/QSVideo/qsvideocamera.h"
#include "src/QSVideo/qsvideofile.h"


class QSFrameViewier : public QLabel
{
    Q_OBJECT
    QTimer                  *redrawTimer = nullptr;

    float widgetAspectRatio = 1.f;
    float frameAspectRatio = 1.f;

    QSVideoUser             *video = nullptr;
    int                     handlerIndex;
    QSVideo::Frame          currentFrame;

    bool                    await = true;
    float                   speed = 1.0f;
    qs::TimeT               redrawTime;
    //WARNING current time isn't equal to actual frame time and used for smooth image
    qs::TimeT               currentTime;
    qs::TimeT               endTime = 0;
    qs::TimeT               endFrame = 0;
    qs::TimeT               prevPauseTime;
    qs::TimeT               prevDrawTime;
    float                   targetFrameRate = 30.f;
    bool                    drawFrameRate;
    float                   actualFrameRate = 0.f;
    static const int        defMinSize = 240;

    struct ZoomParams
    {
        cv::Mat zoomMatrix;
        enum    ZoomState : uint8_t
        {
            NoZoom = 0x0,
            ZoomChoosing,
            Zoom
        };
        ZoomState   state = NoZoom;
        cv::Point2i firstPoint;
        cv::Point2i secondPoint;
        cv::Rect2i  area;
    };

    ZoomParams zp;

public:
    QSFrameViewier(QWidget* parent = nullptr);
    ~QSFrameViewier();

    void                setFrame(const QString &key);

    inline float        getSpeed(){return speed;}
    inline qs::FrameT   getCurrentFrame(){return currentFrame.frame;}
    inline float        getRelativeFrame(){return endFrame == 0 ? 0 :
                                            ((float)currentFrame.frame)/endFrame;}
    inline qs::TimeT    getVideoLength(){return endTime;}
    inline qs::TimeT    getAbsoluteTime(){return currentTime;}
    inline float        getRelativeTime(){return endTime == 0 ? 0 :
                                            ((float)currentTime)/endTime;}
    inline float        getActualFrameRate(){return actualFrameRate;}

    static inline QImage  cvMatToQImage( const cv::Mat &inMat )
       {
          switch ( inMat.type() )
          {
             // 8-bit, 4 channel
             case CV_8UC4:
             {
                QImage image( inMat.data,
                              inMat.cols, inMat.rows,
                              static_cast<int>(inMat.step),
                              QImage::Format_ARGB32 );

                return image;
             }

             // 8-bit, 3 channel
             case CV_8UC3:
             {
                QImage image( inMat.data,
                              inMat.cols, inMat.rows,
                              static_cast<int>(inMat.step),
                              QImage::Format_RGB888 );

                return image.rgbSwapped();
             }

             // 8-bit, 1 channel
             case CV_8UC1:
             {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
                QImage image( inMat.data,
                              inMat.cols, inMat.rows,
                              static_cast<int>(inMat.step),
                              QImage::Format_Grayscale8 );
    #else
                static QVector<QRgb>  sColorTable;

                // only create our color table the first time
                if ( sColorTable.isEmpty() )
                {
                   sColorTable.resize( 256 );

                   for ( int i = 0; i < 256; ++i )
                   {
                      sColorTable[i] = qRgb( i, i, i );
                   }
                }

                QImage image( inMat.data,
                              inMat.cols, inMat.rows,
                              static_cast<int>(inMat.step),
                              QImage::Format_Indexed8 );

                image.setColorTable( sColorTable );
    #endif

                return image;
             }

             default:
                qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
                break;
          }

          return QImage();
       }

private slots:
    void        redraw();
    void        update_frame();
    void        videoReady();

public slots:
    //void        resizeEvent(QResizeEvent* event) override;
    void        paintEvent(QPaintEvent* event) override;
    void        mousePressEvent(QMouseEvent* event) override;
    void        mouseReleaseEvent(QMouseEvent *event) override;

    void        pausePlay();
    void        play();
    void        pause();
    void        setSpeed(float speed);
    void        moveT(int64_t time);
    void        moveF(int64_t frame);
    void        setT(qs::TimeT time);
    void        setF(qs::FrameT time);
signals:
    void        newFrame(qs::TimeT);
};

#endif // QSFRAMEVIEWIER_H
