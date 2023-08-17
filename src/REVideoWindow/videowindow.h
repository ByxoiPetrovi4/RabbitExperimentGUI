#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <thread>
#include <QMainWindow>
#include "../QSFrameViewier/qsframeviewier.h"
#include "../QSVideo/qsframewriter.h"

namespace Ui {
class VideoWindow;
}

class VideoWindow : public QMainWindow
{
    Q_OBJECT
    std::string             video_name;
    QString                 memory_key;
    QSFrameViewier          *viewier;
    QSVideo::VideoParams    params;
    std::thread             *thread;
    float                   videofile_fps = 30.f;
public:
    explicit VideoWindow(QSVideo::VideoParams videoParams, QString key, QWidget *parent = nullptr);
    ~VideoWindow();
public slots:
    void            playPause();
    void            recordStop();
private:
    Ui::VideoWindow *ui;
private slots:
    void            ready();
    void            error(int);
    void            stop();
};

#endif // VIDEOWINDOW_H
