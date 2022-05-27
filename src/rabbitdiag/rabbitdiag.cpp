#include "rabbitdiag.h"
#include "ui_rabbitdiag.h"

void QStringToCStr(const QString qstr, char* cstr, size_t max_size)
{
    QByteArray tmp = qstr.toLatin1();
    strncpy(cstr, tmp.data(), max_size);
    cstr[max_size] = '\0';
}

RabbitDiag::RabbitDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RabbitDiag)
{
    char buf[64];
    ui->setupUi(this);
    FILE* cfgFile = fopen(CONFIG_FILENAME, "r");
    if(cfgFile == 0)
    {
        ui->actorName->addItem(QString("Joka"));
        ui->spectatorName->addItem(QString("Boka"));
        ui->workerName->addItem(QString("Bogdan"));
        qDebug() << std::strerror(errno);
        return;
    }
    int i = 0;
    for(char ch = fgetc(cfgFile); ch == '#' && ch != EOF;
        ch = fgetc(cfgFile))
    {
       fgets(buf, 64, cfgFile);
       buf[strlen(buf)-1] = 0;
       ui->workerName->addItem(QString(buf));
       //ui->workerName->setItemData(i, Qt::AlignVCenter, Qt::TextAlignmentRole);
    }
    i = 0;
    for(char ch = fgetc(cfgFile); ch == '#' && ch != EOF;
          ch = fgetc(cfgFile))
    {
       fgets(buf, 64, cfgFile);
       buf[strlen(buf)-1] = 0;
       ui->actorName->addItem(QString(buf));
       //ui->actorName->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    i = 0;
    for(char ch = fgetc(cfgFile); ch == '#' && ch != EOF;
          ch = fgetc(cfgFile))
    {
       fgets(buf, 64, cfgFile);
       buf[strlen(buf)-1] = 0;
       ui->spectatorName->addItem(QString(buf));
       //ui->spectatorName->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    fclose(cfgFile);
}

RabbitDiag::~RabbitDiag()
{
    delete ui;
}

Info_Settings RabbitDiag::settings()
{
    currentSettings.learning = ui->learningCheck->isChecked();
    QStringToCStr(ui->actorName->currentText(),
                  currentSettings.actorName, 64);
    QStringToCStr(ui->spectatorName->currentText(),
                  currentSettings.spectatorName, 64);
    QStringToCStr(ui->workerName->currentText(),
                  currentSettings.workerName, 64);
    QStringToCStr(ui->experimentDate->date().toString(),
                  currentSettings.date, 64);
    currentSettings.actorWeight = ui->actorWeight->value();
    currentSettings.spectatorWeight = ui->spectatorWeight->value();
    return currentSettings;
}

void RabbitDiag::on_setupTimeButton_clicked()
{
    ui->experimentDate->setDate(QDate::currentDate());
    ui->experimentDate->setTime(QTime::currentTime());
}

