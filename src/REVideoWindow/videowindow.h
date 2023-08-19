#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <thread>
#include <QMainWindow>
#include <QMessageBox>
#include <QCloseEvent>
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
    explicit VideoWindow(QSVideo::VideoParams videoParams, QString key, QString filePath = "/tmp/live.avi",
                         float record_fps = 30.f, QWidget *parent = nullptr);
    ~VideoWindow();
private:
signals:
    void            kill();
public slots:
    void            playPause();
    void            recordStop();
    void            fps(float);
private:
    Ui::VideoWindow *ui;
private slots:
    void            ready();
    void            error(int);
    void            stop();
    void            closeEvent(QCloseEvent *event) override;
};

#endif // VIDEOWINDOW_H
