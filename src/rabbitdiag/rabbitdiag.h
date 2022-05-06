#ifndef RabbitDIAG_H
#define RabbitDIAG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class RabbitDiag;
}

class RabbitDiag : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString     worker_name;
        QString     actor_name;
        QString     spectator_name;
        bool        learning_mode;
        qint32      actor_weight;
        qint32      spectator_weight;
        QDateTime   date;
    };
    explicit RabbitDiag(QWidget *parent = nullptr);
    ~RabbitDiag();

    Settings settings() const;

private:
    void updateSettings();

private:
    Ui::RabbitDiag *ui;
    Settings currentSettings;
};

#endif // RabbitDIAG_H
