#include "wrwindow.h"
#include "ui_wrwindow.h"

wrwindow::wrwindow(CameraWriter* cw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wrwindow)
{
    ui->setupUi(this);
    camwr = cw;
    connect(camwr, SIGNAL(newFrame(const QPixmap&)), ui->label,
            SLOT(setPixmap(const QPixmap&)));
    connect(camwr, SIGNAL(fpsRate(const QString&)), ui->fpsLabel,
            SLOT(setText(const QString&)));
    //connect(this, SIGNAL(close()), &camwr, SLOT(stop()));
}

wrwindow::~wrwindow()
{
    delete ui;
}

void wrwindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Rabbit Camera"),
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            camwr->stop();
            event->accept();
        }
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

