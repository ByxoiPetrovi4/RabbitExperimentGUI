#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>
#include <QMessageBox>
#include "serialdiag.h"
#include "rabbitdiag.h"
#include "data_handler.h"
#include "reserial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showStatusMessage(const QString &);

public slots:
    void statusChange(QString);
    void stateChange(RESerial::ProcessState);
    void newMessage(QString);

    void on_actionSerial_settings_triggered();

    void on_actionRecord_settings_triggered();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui = nullptr;
    SerialDiag*     serialDialog = nullptr;
    RabbitDiag*     rabbitDialog = nullptr;
    RESerial*       reSerial = nullptr;

    bool connected;
};
#endif // MAINWINDOW_H
