#include "wrwindow.h"
#include "ui_wrwindow.h"

wrwindow::wrwindow(CameraWriter* cw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wrwindow)
{
    ui->setupUi(this);
    camwr = cw;
    killable = false;
    connect(camwr, SIGNAL(newFrame(const QImage&)), this,
            SLOT(frameUpdate(const QImage&)));
    connect(camwr, SIGNAL(fpsRate(const QString&)), ui->fpsLabel,
            SLOT(setText(const QString&)));
    connect(camwr, SIGNAL(finished()), this,
            SLOT(close()));
}

wrwindow::~wrwindow()
{
    delete ui;
}

void wrwindow::kill()
{
    camwr->stop();
    killable = true;
}

void wrwindow::closeEvent(QCloseEvent *event)
{
    if(killable)
    {
        event->accept();
        return;
    }
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Rabbit Camera"),
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
            camwr->stop();
            killable = true;
        }
    event->ignore();
}

void wrwindow::on_displayCBox_stateChanged(int arg1)
{
    camwr->display(arg1 ? true : false);
}


void wrwindow::on_writeButton_clicked()
{
    if(camwr->isWriting())
    {
        camwr->write(false);
        ui->writeButton->setText("Write");
        return;
    }
    camwr->write(true);
    ui->writeButton->setText("Stop");
}

void wrwindow::frameUpdate(const QImage& img)
{
    QPixmap t = QPixmap::fromImage(img);
    ui->label->setPixmap(t.scaled(960, 540, Qt::KeepAspectRatio));
}
