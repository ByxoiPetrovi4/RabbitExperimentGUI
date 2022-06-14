#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>
#include <QMessageBox>
#include <QShortcut>
#include "serialdiag.h"
#include "rabbitdiag.h"
#include "data_handler.h"
#include "reserial.h"
#include "wrwindow.h"

#define RE_CONFIG_FILENAME "defsettings.cfg"
#define RE_COMMENT_SYMBOL  '#'

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(CameraWriter*, wrwindow*, QWidget *parent = nullptr);
    ~MainWindow();
    void showStatusMessage(const QString &);

signals:
    void startDate();

public slots:
    void statusChange(QString);
    void stateChange(RESerial::ProcessState);
    void newMessage(QString);
    void settingsReceived(RE_Settings);

    void on_actionSerial_settings_triggered();

    void on_actionRecord_settings_triggered();

    void on_connectButton_clicked();

private slots:
    void closeEvent(QCloseEvent*) override;

    void experimentStop();

    void on_soundButton_clicked();

    void on_startButton_clicked();

    void on_actionSave_at_triggered();

    void on_actionVideo_settings_triggered();

    void on_cameraButton_clicked();

    void on_feedButton_clicked();

    void on_settingsButton_clicked();

    void on_pauseButton_clicked();

    void on_feedErrorButton_clicked();

private:
    Ui::MainWindow *ui = nullptr;
    SerialDiag*     serialDialog = nullptr;
    RabbitDiag*     rabbitDialog = nullptr;
    RESerial*       reSerial = nullptr;
    CameraWriter*   cmwr = nullptr;
    wrwindow*       cameraWindow = nullptr;

    QShortcut*      hkeyFeed = nullptr;
    QShortcut*      hkeyEFeed = nullptr;
    QShortcut*      hkeySound = nullptr;
    QShortcut*      hkeyPause = nullptr;

    bool connected;
    bool readyToStart;
    bool experimentActive;
    QString saveDirectory;

    void experimentStart();
    void readConfig();
};
#endif // MAINWINDOW_H
