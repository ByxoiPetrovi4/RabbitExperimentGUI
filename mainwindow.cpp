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
    readyToStart = false;
    //Warning connections through dll with cmake works only with macroses!
    connect(reSerial, SIGNAL(statusChange(QString)), this,
            SLOT(statusChange(QString)));
    connect(reSerial, SIGNAL(stateChange(RESerial::ProcessState)),
                             this, SLOT(stateChange(RESerial::ProcessState)));
    connect(reSerial, SIGNAL(newMessage(QString)),
            this, SLOT(newMessage(QString)));
    connect(reSerial, SIGNAL(settingsReceived(RE_Settings)),
            this, SLOT(settingsReceived(RE_Settings)));
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
            ui->connectButton->setText(tr("Send settings"));
            ui->settingsBox->setEnabled(false);
            ui->connectButton->setText(tr("Connect"));
            connected = false;
            break;
        case RESerial::AwaitAnswer:
            break;
        case RESerial::AwaitEvent:
            ui->settingsBox->setEnabled(true);
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

void MainWindow::settingsReceived(RE_Settings set)
{
    ui->startButton->setText("Start");
    ui->settingsBox->setEnabled(false);
    readyToStart = true;
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
    reSerial->sendCommand(KBeep);
}


void MainWindow::on_startButton_clicked()
{
    if(readyToStart)
    {
        reSerial->sendCommand(KStart);
        return;
    }
    RE_Settings st;
    st.food = 100;
    st.press_interval = ui->pressIntervalSPBox->value()*1000;
    st.sound_length = 2500;
    st.manual = ui->manualCheck->isChecked() ? 1 : 0;
    st.min_delay = ui->minDelaySPBox->value()*1000;
    st.max_delay = ui->maxDelaySPBox->value()*1000;
    reSerial->sendSettings(st);
}


void MainWindow::on_actionSave_at_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "\\",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    QStringToCStr(dir, save_directory, 128);
}


void MainWindow::on_actionVideo_settings_triggered()
{

}

