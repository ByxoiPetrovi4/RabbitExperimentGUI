#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialDialog = new SerialDiag(this);
    rabbitDialog = new RabbitDiag(this);
    reSerial = new RESerial(this);

    connected = false;
    //Warning connections through dll with cmake works only with macroses!
    connect(reSerial, SIGNAL(statusChange(QString)), this,
            SLOT(statusChange(QString)));
    connect(reSerial, SIGNAL(stateChange(RESerial::ProcessState)),
                             this, SLOT(stateChange(RESerial::ProcessState)));
    connect(reSerial, SIGNAL(newMessage(QString)),
            this, SLOT(newMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::statusChange(QString str)
{
    ui->connectStatus->setText(str);
}

void MainWindow::stateChange(RESerial::ProcessState st)
{
    switch(st)
    {
        case RESerial::NoConnect:
            statusChange(tr("Not connected"));
            ui->connectButton->setText(tr("Connect"));
            connected = false;
            break;
        case RESerial::AwaitAnswer:
        case RESerial::AwaitEvent:
            ui->connectButton->setText(tr("Disconnect"));
            connected = true;
            break;
    }
}

void MainWindow::newMessage( QString txt)
{
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(txt);
}

void MainWindow::on_actionSerial_settings_triggered()
{
    serialDialog->show();
}


void MainWindow::on_actionRecord_settings_triggered()
{
    rabbitDialog->show();

}


void MainWindow::on_connectButton_clicked()
{
    if(connected)
    {
        if(QMessageBox::question(this, "Rabbit Experiment Warning",
                "Do you really want to disconnect device?",
                QMessageBox::Yes|QMessageBox::No) ==
                QMessageBox::No)return;
        reSerial->Disconnect();
        connected = false;
        ui->connectButton->setText(tr("Connect"));
        return;
    }
    reSerial->Connect(serialDialog->settings());
}


void MainWindow::on_soundButton_clicked()
{
    reSerial->sendCommand(KSound);
}

