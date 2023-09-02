#include "videowindow.h"
#include "ui_videowindow.h"

VideoWindow::VideoWindow(QSVideo::VideoParams videoParams, QString key, QString filePath,
                         float record_fps, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoWindow), params(videoParams), memory_key(key)
{
    USE_CHLIT
    ui->setupUi(this);
    viewier = new QSFrameViewier(this);
    record = std::make_shared<int>(1);
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
        if(!connect(this, SIGNAL(kill()), &cam, SLOT(kill())))
        {
            qDebug() << "Kill isn't connected! \n key =" << memory_key;
            return;
        }
        if(!connect(&cam, SIGNAL(fps(float)), this, SLOT(fps(float))))
        {
            qWarning() << "Can't display fps!";
        }
        cam.ready();
        params_cpy.fps = record_fps;
        auto fp = filePath.toStdString();
        QSFrameWriter fwriter(fp, params_cpy);
        auto start = cam.getStartTime();
        auto dur =
                std::chrono::high_resolution_clock::now() - start;
        const qs::TimeT frame_time = 1000000.f/params_cpy.fps;
        auto durus = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
        for(uint32_t i = 0; ; i++)
        {

            auto upd = cam.update();
            if(upd == QSVideo::QSV_NEW && *record!=0)
            {
                auto fr = cam.getFrameL();
                if(fr.time > durus)
                {
                    durus += frame_time;
                    fwriter.write(fr);
                }
            }   else if(upd == QSVideo::QSV_EXIT)
            {
                break;
            }
        }
        qDebug() << "Correct end";
    });
    thread->detach();
    ui->mainLayout->addWidget(viewier);
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

void            VideoWindow::fps(float f)
{
    ui->fpsLabel->setText("FPS: " + QString::number(f, 'g', 3));
}

void            VideoWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, windowTitle(),
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            delete viewier;
            event->accept();
        }
}

void VideoWindow::on_recordButton_clicked()
{
    if(*record)
    {
        *record = 0;
        ui->recordButton->setText("Continue record");
    }
    else
    {
        ui->recordButton->setText("Pause record");
        *record = 1;
    }
}


void VideoWindow::on_playButton_clicked()
{

}

