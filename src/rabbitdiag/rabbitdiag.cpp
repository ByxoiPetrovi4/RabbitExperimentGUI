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
    std::fstream cfg_stream;
    cfg_stream.open(REDIAG_CONFIG_FILENAME, std::ios_base::in);
    if(!cfg_stream.is_open())
    {
        qDebug() << "Config couldn't be read!";
        return;
    }
    nlohmann::json config;
    try {
        cfg_stream >> config;
    } catch (std::exception &exc) {
        qDebug() << exc.what();
        return;
    }
    cfg_stream.close();
    if(!config["Workers"].is_array())
        qWarning() << "REDiag config no workers defind!";
    else
        for(auto &&i : config["Workers"])
        {
            if(!i.is_string())
                break;
            ui->workerName->addItem(i.get<std::string>().c_str());
        }
    if(!config["Actors"].is_array())
        qWarning() << "REDiag config no actors defind!";
    else
        for(auto &&i : config["Actors"])
        {
            if(!i.is_string())
                break;
            ui->actorName->addItem(i.get<std::string>().c_str());
        }
    if(!config["Spectators"].is_array())
        qWarning() << "REDiag config no spectators defind!";
    else
        for(auto &&i : config["Spectators"])
        {
            if(!i.is_string())
                break;
            ui->spectatorName->addItem(i.get<std::string>().c_str());
        }
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

