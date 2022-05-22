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
        State = NoConnect;
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

void RESerial::Disconnect()
{
    sendCommand(KClose);
    QThread::msleep(100);
    close();
    emit statusChange(tr("Disconnected"));
    serialTimer->stop();
    State = NoConnect;
}

void RESerial::sendCommand(Keywords k)
{
    char buf[4] = {Command, k, End, 0};
    qDebug() << buf;
    lastTMessage = QByteArray(buf, 3);
    State = AwaitAnswer;
    write(lastTMessage);
    serialTimer->start(RES_DEFAULT_RESEND_INTERVAL);
}

void RESerial::sendSettings(RE_Settings st)
{
    char buf[64];
    buf[0] = ':';
    buf[1] = 's';
    SettingsToStr(st, buf+2);
    lastTMessage = QByteArray(buf, strlen(buf));
    State = AwaitAnswer;
    write(lastTMessage);
    serialTimer->start(RES_DEFAULT_RESEND_INTERVAL);
}

void RESerial::processNC(void)
//Connection pattern send '~''~''~' and w8 for same answer
//if after RES_DEFAULT_MAX_FAILS connection not estabileshed then
//signal resendFails emit
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
//Processing in await answer from device
//occures only after command send.
//Awaiting to receive same last transmitted message
{
    if(!canReadLine())return;
    char buf[64];
    uint16_t len = readLine(buf, 64);
    qDebug() << buf;
    __re_abstract_data proc_data = process_message(buf,len);
    if(proc_data.typ == DTAnswer)
    {
        if(memcmp(buf+1, lastTMessage.data()+1, len-1))return;
        State = AwaitEvent;
        serialTimer->stop();
    }
    if(proc_data.typ >= DTSettings)
    {
        if(memcmp(buf+1, lastTMessage.data()+1, len-1))return;
        State = AwaitEvent;
        serialTimer->stop();
        emit settingsReceived(Settings);
        qDebug() << "Settings is read";
        //add .log output
        return;
    }
    if(proc_data.typ == DTError)
    {
        emit statusChange(tr(ToStr(proc_data)));
        //redact and add writing to .log
        return;
    }
    retimeToStr(GetTimeStamp(), 0, buf);
    newMessage(tr(buf) + " " + ToStr(proc_data) + "\n");
}

void RESerial::processAE(void)
//Proceesing in await event or command from device.
//if error in message found then error will send to device
//awaiting resending for the previous message.
//if all correct state doesn't change.
{
    if(!canReadLine())return;
    char buf[64];
    uint16_t len = readLine(buf, 64);
    qDebug() << buf << '\n';
    __re_abstract_data proc_data = process_message(buf,len);
    if(proc_data.typ == DTError)
    {
        emit statusChange(tr(ToStr(proc_data)));
        //redact and add writing to .log
        return;
    }
    retimeToStr(GetTimeStamp(), 0, buf);
    newMessage(tr(buf) + " " + ToStr(proc_data) + "\n");
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
