#include "videowindow.h"
#include "ui_videowindow.h"

VideoWindow::VideoWindow(QSVideo::VideoParams videoParams, QString key, QString filePath,
                         float record_fps, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoWindow), params(videoParams), memory_key(key)
{
    ui->setupUi(this);
    viewier = new QSFrameViewier(this);
    thread = new std::thread([this, filePath, record_fps]()
    {
        //TODO: add check for source!
        auto params_cpy = params;
        QSVideoCamera cam(memory_key, params_cpy);
        if(!connect(&cam, SIGNAL(ready()), this, SLOT(ready())))
        {
            qDebug() << "Ready isn't connected! \n key =" << memory_key;
            return;
        }
        cam.ready();
        qDebug() << params_cpy.fwidth << params_cpy.fheight;
        params_cpy.fps = record_fps;
        auto fp = filePath.toStdString();
        QSFrameWriter fwriter(fp, params_cpy);
        QSVideo::Frame fr;
        fr.time = 0;
        const qs::TimeT frame_time = 1000000.f/params_cpy.fps;
        qDebug() << frame_time;
        while(1)
        {
            auto upd = cam.update();
            if(upd == QSVideo::QSV_NEW)
            {
                cam.getFrameT(fr, fr.time + frame_time);
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
