#include "reserial.h"

RESerial::RESerial(QObject *parent)
    : QSerialPort{parent}
{
    serialTimer = new QTimer(parent);
    serialTimer->setSingleShot(true);

    connect(serialTimer, &QTimer::timeout,
            this, &RESerial::timerTick);
    connect(this, &QSerialPort::readyRead,
            this, &RESerial::dataProcess);

    messages_received = 0;
    messages_sent = 0;
    Settings = defaultRE_Settings;
    fails = 0;
    max_fails = RES_DEFAULT_MAX_FAILS;
    State = NoConnect;
}

void RESerial::Connect(const SerialDiag::Settings st)
{
    setPortName(st.name);
    setBaudRate(st.baudRate);
    setDataBits(st.dataBits);
    setParity(st.parity);
    setStopBits(st.stopBits);
    setFlowControl(st.flowControl);
    if(open(QIODevice::ReadWrite))
    {
        emit statusChange(tr("Connected to %1 : %2, %3, %4, %5, %6")
                     .arg(st.name).arg(st.stringBaudRate).arg(st.stringDataBits)
                     .arg(st.stringParity).arg(st.stringStopBits).arg(st.stringFlowControl));
        lastTMessage = QByteArray("~~~", 3);
        fails = 0;
        serialTimer->start(5000);
    }
    else
    {
        QMessageBox::critical(0, tr("Error"), errorString());
        emit statusChange(tr("Open error"));
    }
}

void RESerial::processNC(void)
{
    qint64 av = bytesAvailable();
    if(av < RES_MINIMUM_LENGTH)return;
    if(av != 3)
    {
        readAll();
        serialTimer->start(RES_DEFAULT_RESEND_INTERVAL);
    }
    char buf[3];
    read(buf, 3);
    if(buf[0] == '~' && buf[1] == '~' && buf[2] == '~')
    {
        State = AwaitEvent;
        serialTimer->stop();
        lastRMessage = QByteArray(buf, 3);
        emit stateChange(State);
        emit statusChange(tr("Connected to device!"));
        return;
    }
    serialTimer->start(RES_DEFAULT_RESEND_INTERVAL);
}

void RESerial::processAA(void)
{

}

void RESerial::processAE(void)
{
    qint64 av = bytesAvailable();
    if(av < RES_MINIMUM_LENGTH)return;
    char buf[3];
    read(buf, 3);
    uint16_t pos = 1;
    newMessage(tr(ToStr(process_event(buf,pos,3))));
}

void RESerial::dataProcess()
{
    switch(State)
    {
        case NoConnect:
            processNC();
            break;
        case AwaitAnswer:
            processAA();
            break;
        case AwaitEvent:
            processAE();
            break;
    }
}

void RESerial::timerTick()
{
    if(fails>max_fails)
    {
        //resendFails();
        return;
    }
    write(lastTMessage);
    fails++;
    serialTimer->start(RES_DEFAULT_RESEND_INTERVAL);
}
