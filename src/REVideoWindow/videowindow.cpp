#include "videowindow.h"
#include "ui_videowindow.h"

VideoWindow::VideoWindow(QSVideo::VideoParams videoParams, QString key, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoWindow), params(videoParams), memory_key(key)
{
    ui->setupUi(this);
    viewier = new QSFrameViewier(this);
    thread = new std::thread([this]()
    {
        //TODO: add check for source!
        QSVideoCamera cam(memory_key, params);
        if(!connect(&cam, SIGNAL(ready()), this, SLOT(ready())))
        {
            qDebug() << "Ready isn't connected! \n key =" << memory_key;
            return;
        }
        cam.ready();
        auto param_cpy = params;
        param_cpy.fps = videofile_fps;
        QSFrameWriter fwriter("/tmp/live.mkv", param_cpy);
        QSVideo::Frame fr;
        fr.time = 0;
        while(cam.getState() != QSVideo::QSV_KILL)
        {
            auto upd = cam.update();
            if(upd == QSVideo::QSV_NEW)
            {
                cam.getFrameT(fr, fr.time + 100000/param_cpy.fps);
                fwriter.write(fr);
            }   else if(upd == QSVideo::QSV_EXIT)
            {
                break;
            }
        }
        qDebug() << "Correct end";
    });
    thread->detach();
    ui->mainLayout->addWidget(viewier);
    ui->groupBox->setEnabled(false);
}

VideoWindow::~VideoWindow()
{
    USE_CHLIT
    delete ui;
    delete viewier;
    std::this_thread::sleep_for(100ms);
    delete thread;
}

void            VideoWindow::playPause()
{

}

void            VideoWindow::recordStop()
{

}

void            VideoWindow::ready()
{
    qDebug() << "Memory is rdy!";
    viewier->setFrame(memory_key);
    viewier->play();
}

void            VideoWindow::error(int)
{

}

void            VideoWindow::stop()
{

}
