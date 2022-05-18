#ifndef RESERIAL_H
#define RESERIAL_H

#include <cstdint>

#include <QSerialPort>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QtDebug>
#include <QThread>

#include "serialdiag.h"
#include "protocol.h"
#include "data_handler.h"

#define RES_DEFAULT_RESEND_INTERVAL 3000
#define RES_DEFAULT_MAX_FAILS       3
#define RES_MINIMUM_LENGTH          3

const RE_Settings defaultRE_Settings = {0, 3000, 3500, 1, 5000, 10000};

class RESerial : public QSerialPort
{
    Q_OBJECT
public:
    enum ProcessState {NoConnect, AwaitAnswer, AwaitEvent};

    explicit RESerial(QObject *parent = nullptr);

    void Connect(const SerialDiag::Settings);
    void Disconnect();

    void OpenOutput(QString);
    void CloseOutput();

    RE_Settings settings();
    void setSettings(const RE_Settings);
    void sendCommand(Keywords);
    void sendSettings(RE_Settings);

public slots:
    void timerTick();
    void dataProcess();

signals:
    void newMessage(QString);
    void statusChange(QString);
    void stateChange(RESerial::ProcessState);
    void settingsReceived(RE_Settings);
    void resendFails();
    //void endOfFood();

private:
    RE_Settings Settings;
    ProcessState State;

    void processNC(void);
    void processAA(void);
    void processAE(void);

    QTimer* serialTimer = nullptr;

    QByteArray  lastRMessage;
    QByteArray  lastTMessage;

    qint64      messages_received;
    qint64      messages_sent;

    uint8_t     fails;
    uint8_t     max_fails;

    uint16_t    resend_interval;
};

#endif // RESERIAL_H
