#include "rabbitdiag.h"
#include "ui_rabbitdiag.h"

inline void QStringToCStr(const QString qstr, char* cstr, size_t max_size)
{
    QByteArray tmp = qstr.toLatin1();
    strncpy(cstr, tmp.data(), max_size);
    cstr[max_size] = '\0';
}

RabbitDiag::RabbitDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RabbitDiag)
{
    ui->setupUi(this);
    ui->spectatorName->addItem(tr("None"));
    //added for case without spectator

    ui->experimentDate->setDate(QDate::currentDate());

    readConfig();
}

RabbitDiag::~RabbitDiag()
{
    delete ui;
}

void RabbitDiag::readConfig()
{
    char buf[64];
    FILE* cfgFile = fopen(REDIAG_CONFIG_FILENAME, "r");
    if(cfgFile == 0)
    {
        ui->actorName->addItem(QString("Joka"));
        ui->spectatorName->addItem(QString("Boka"));
        ui->workerName->addItem(QString("Bogdan"));
        qDebug() << std::strerror(errno);
        return;
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->workerName->addItem(QString(buf));
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->actorName->addItem(QString(buf));
       }
    }
    for(char* p = fgets(buf, 64, cfgFile); buf[0]!='\n' || buf[0] == EOF;
        fgets(buf, 64, cfgFile))
    {
       if(buf[0]!=REDIAG_COMMENT_SYMBOL || p == 0)
       {
           buf[strlen(buf)-1] = 0;
           ui->spectatorName->addItem(QString(buf));
       }
    }
    fclose(cfgFile);
}

Info_Settings RabbitDiag::settings()
{
    currentSettings.learning = ui->learningCheck->isChecked();
    QStringToCStr(ui->actorName->currentText(),
                  currentSettings.actorName, 32);
    QStringToCStr(ui->spectatorName->currentText(),
                  currentSettings.spectatorName, 32);
    QStringToCStr(ui->workerName->currentText(),
                  currentSettings.workerName, 32);
    QStringToCStr(ui->experimentDate->date().toString(),
                  currentSettings.date, 32);
    currentSettings.actorWeight = ui->actorWeight->value();
    currentSettings.spectatorWeight = ui->spectatorWeight->value();
    return currentSettings;
}

QString RabbitDiag::getSubdirectory()
{
    settings();
    QString out = tr("%1_%2_%3").arg(tr(currentSettings.spectatorName))
                .arg(tr(currentSettings.actorName)).arg(currentSettings.learning ? 'l' : 'e');
    return out;
}

void RabbitDiag::on_setupTimeButton_clicked()
{
    ui->experimentDate->setDate(QDate::currentDate());
    ui->experimentDate->setTime(QTime::currentTime());
}

