#ifndef RabbitDIAG_H
#define RabbitDIAG_H

#include <QDialog>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <iostream>
#include <cerrno>
#include "protocol.h"

#define CONFIG_FILENAME "rediag.cfg"
#define NEW_ITEM_SYMBOL '#'

namespace Ui {
class RabbitDiag;
}

class RabbitDiag : public QDialog
{
    Q_OBJECT

public:
    explicit RabbitDiag(QWidget *parent = nullptr);
    ~RabbitDiag();

    Info_Settings settings();

private slots:
    void on_setupTimeButton_clicked();

private:
    void updateSettings();

private:
    Ui::RabbitDiag *ui;
    Info_Settings currentSettings;
};

void QStringToCStr(const QString, char*, size_t);

#endif // RabbitDIAG_H
