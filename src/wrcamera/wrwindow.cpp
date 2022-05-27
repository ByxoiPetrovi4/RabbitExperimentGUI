#include "wrwindow.h"
#include "ui_wrwindow.h"

wrwindow::wrwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wrwindow)
{
    ui->setupUi(this);
    camwr.start(QThread::HighestPriority);
    connect(&camwr, SIGNAL(newFrame(const QPixmap&)), ui->label,
            SLOT(setPixmap(const QPixmap&)));
    connect(&camwr, SIGNAL(fpsRate(const QString&)), ui->fpsLabel,
            SLOT(setText(const QString&)));
    //connect(this, SIGNAL(close()), &camwr, SLOT(stop()));
}

wrwindow::~wrwindow()
{
    delete ui;
}

void wrwindow::on_displayCBox_stateChanged(int arg1)
{
    camwr.display(arg1 ? true : false);
}


void wrwindow::on_writeButton_clicked()
{
    if(camwr.isWriting())
    {
        camwr.write(false);
        ui->writeButton->setText("Write");
        return;
    }
    camwr.write(true);
    ui->writeButton->setText("Stop");
}

