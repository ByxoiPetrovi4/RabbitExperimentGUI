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
    dsTimer  = new QTimer(this);

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
    connect(reSerial, SIGNAL(settingsReceiveError()),
            this, SLOT(settingsReceiveError()));
    connect(reSerial, SIGNAL(endOfFood()), this,
            SLOT(experimentStop()));
    connect(this, SIGNAL(startDate()),
            reSerial, SLOT(writeStartTime()));
    connect(dsTimer, SIGNAL(timeout()), this,
            SLOT(on_sdTimercount()));

    dsTimer->setInterval(60000);
    dsTimer->setSingleShot(false);
    dsTimer->start();
    on_sdTimercount();

    readConfig();
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
            ui->cameraButton->setEnabled(true);
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
    if(txt.section(' ', 2, 2 ).toUInt() == KPause)
    {
        if(experimentActive)
        {
            experimentActive = false;
            ui->feedButton->setEnabled(false);
            ui->soundButton->setEnabled(false);
            ui->feedErrorButton->setEnabled(false);
            ui->pauseButton->setText("Unpause");
        }
        else
        {
            experimentActive = true;
            ui->feedButton->setEnabled(true);
            ui->soundButton->setEnabled(true);
            ui->feedErrorButton->setEnabled(true);
            ui->pauseButton->setText("Pause");
        }
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

void MainWindow::settingsReceiveError()
{
    QMessageBox::warning(this, "Rabbit Experiment Warning",
                    "Receiving settings error!\nReplug USB to Arduino!",
                         QMessageBox::Ok);
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
        if(reSerial->getState() != RESerial::NoConnect) experimentStop();
        reSerial->Disconnect();
        connected = false;
        ui->connectButton->setText(tr("Connect"));
        ui->cameraButton->setEnabled(false);
        return;
    }
    try {
        QString subDir = QDateTime::currentDateTime().toString(Qt::ISODate) +
                "_" + rabbitDialog->getSubdirectory();
        reSerial->setWorkDir(saveDirectory, subDir);
        cmwr->setDir(saveDirectory + "/" + subDir);
        reSerial->Connect(serialDialog->settings());
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
}

void MainWindow::on_startButton_clicked()
{
    if(readyToStart && !experimentActive)
    {
        reSerial->writeExpInfo(rabbitDialog->settings());
        reSerial->sendCommand(KStart);
        emit startDate();
        experimentStart();
        return;
    }
    if(experimentActive)
    {
        experimentStop();
    }
}


void MainWindow::on_actionSave_at_triggered()
{
    saveDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "\\",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
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

void MainWindow::on_sdTimercount()
{
    statvfs64("/", &diskStats);
    freeSpace = diskStats.f_bavail*4096ull/1000000000.0;
    ui->spaceLabel->setText(tr("Free space: ") + QString::number(freeSpace) + " Gb");
    if(freeSpace < 5.0)
    {
        QMessageBox::warning(this, tr("Rabbit experiment"), tr("Low on freespace autostop on 1 Gb"));
    }
    if(freeSpace < 1.0)
    {
        experimentStop();
    }
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
    reSerial->closeOutput();
    cameraWindow->kill();
}

void MainWindow::readConfig()
{
    FILE* cfgFile = fopen(RE_CONFIG_FILENAME, "r");
    if(cfgFile == 0)
    {
        qDebug() << std::strerror(errno);
        return;
    }
    char buf[127];
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->manualCheck->setChecked(buf[0] != 0);
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->soundLengthSPBox->setValue(strtod(buf, 0));
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->pressIntervalSPBox->setValue(strtod(buf, 0));
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->maxDelaySPBox->setValue(strtod(buf, 0));
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 32, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->minDelaySPBox->setValue(strtod(buf, 0));
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 32, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->foodSPBox->setValue(strtol(buf, 0, 0));
           break;
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 32, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           saveDirectory = tr(buf);
           break;
       }
    }
    fclose(cfgFile);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Rabbit Experiment"),
                tr("Are you sure?\n"), QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
        return;
    }
    if(connected)reSerial->Disconnect();
    cameraWindow->kill();
    event->accept();
}
