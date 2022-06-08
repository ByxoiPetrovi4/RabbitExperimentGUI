#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(CameraWriter* cw, wrwindow* wrw, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cameraWindow = wrw;
    cmwr = cw;
    serialDialog = new SerialDiag(this);
    rabbitDialog = new RabbitDiag(this);
    reSerial = new RESerial(this);

    hkeyFeed = new QShortcut(this);
    hkeyEFeed = new QShortcut(this);
    hkeySound = new QShortcut(this);
    hkeyPause = new QShortcut(this);

    hkeyFeed->setKey(Qt::CTRL | Qt::Key_F);
    hkeyEFeed->setKey(Qt::CTRL | Qt::Key_M);
    hkeySound->setKey(Qt::CTRL | Qt::Key_S);
    hkeyPause->setKey(Qt::CTRL | Qt::Key_P);

    connect(hkeyFeed, SIGNAL(activated()),
            this, SLOT(on_feedButton_clicked()));
    connect(hkeyEFeed, SIGNAL(activated()),
            this, SLOT(on_feedErrorButton_clicked()));
    connect(hkeySound, SIGNAL(activated()),
            this, SLOT(on_soundButton_clicked()));
    connect(hkeyPause, SIGNAL(activated()),
            this, SLOT(on_pauseButton_clicked()));

    connected = false;
    readyToStart = false;

    experimentActive = false;
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
    if(txt.count(' ')<2)
    {
        ui->plainTextEdit->insertPlainText(txt);
        return;
    }
    //check for enum output, if it's there than don't show enum
    ui->plainTextEdit->insertPlainText(txt.section(' ', 0, 1) + "\n");
}

void MainWindow::settingsReceived(RE_Settings set)
{
    ui->startButton->setText("Start");
    ui->startButton->setEnabled(true);
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
        experimentStop();
        reSerial->Disconnect();
        connected = false;
        ui->connectButton->setText(tr("Connect"));
        return;
    }
    try {
        reSerial->Connect(serialDialog->settings());
        ui->settingsBox->setEnabled(true);
    }  catch (QString exp) {

    }

}

void MainWindow::on_soundButton_clicked()
{
    if(!experimentActive)return;
    reSerial->sendCommand(KBeep);
}

void MainWindow::on_feedButton_clicked()
{
    if(!experimentActive)return;
    reSerial->sendCommand(KFeed);
}

void MainWindow::on_feedErrorButton_clicked()
{
    if(!experimentActive)return;
    reSerial->sendCommand(KEFeed);
}

void MainWindow::on_pauseButton_clicked()
{
    reSerial->sendCommand(KPause);
    if(experimentActive)
    {
        experimentActive = false;
        ui->feedButton->setEnabled(false);
        ui->soundButton->setEnabled(false);
        ui->feedErrorButton->setEnabled(false);

        ui->pauseButton->setText("Unpause");
        return;
    }
    experimentActive = true;
    ui->feedButton->setEnabled(true);
    ui->soundButton->setEnabled(true);
    ui->feedErrorButton->setEnabled(true);

    ui->pauseButton->setText("Pause");
}

void MainWindow::on_startButton_clicked()
{
    if(readyToStart && !experimentActive)
    {
        reSerial->writeExpInfo(rabbitDialog->settings());
        reSerial->sendCommand(KStart);
        experimentStart();
        return;
    }
    if(experimentActive)
    {
        reSerial->sendCommand(KClose);
        experimentStop();
    }
}


void MainWindow::on_actionSave_at_triggered()
{
    saveDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "\\",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    reSerial->setWorkDir(saveDirectory, QDateTime::currentDateTime().toString(Qt::ISODate) +
                         "_" + rabbitDialog->getSubdirectory());
}


void MainWindow::on_actionVideo_settings_triggered()
{

}


void MainWindow::on_cameraButton_clicked()
{
    cmwr->start();
    cameraWindow->show();
}


void MainWindow::on_settingsButton_clicked()
{
    RE_Settings st;
    st.food = ui->foodSPBox->value();
    st.press_interval = ui->pressIntervalSPBox->value()*1000;
    st.sound_length = ui->soundLengthSPBox->value()*1000;
    st.manual = ui->manualCheck->isChecked() ? 1 : 0;
    st.min_delay = ui->minDelaySPBox->value()*1000;
    st.max_delay = ui->maxDelaySPBox->value()*1000;
    reSerial->sendSettings(st);
}

void MainWindow::experimentStart()
{
    experimentActive = true;
    ui->feedButton->setEnabled(true);
    ui->soundButton->setEnabled(true);
    ui->feedErrorButton->setEnabled(true);

    ui->pauseButton->setEnabled(true);
    ui->startButton->setText("Stop");
    ui->settingsBox->setEnabled(false);
}

void MainWindow::experimentStop()
{
    experimentActive = false;
    ui->feedButton->setEnabled(false);
    ui->soundButton->setEnabled(false);
    ui->feedErrorButton->setEnabled(false);

    ui->pauseButton->setEnabled(false);
    ui->startButton->setEnabled(false);
}


