#include "reserial.h"

inline void QStringToCStr(const QString qstr, char* cstr, size_t max_size)
{
    QByteArray tmp = qstr.toLatin1();
    strncpy(cstr, tmp.data(), max_size);
    cstr[max_size] = '\0';
}

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
    try {
        openOutput();
    }  catch (QString out) {
        QMessageBox::critical(0, tr("Error"), out);
        emit statusChange(out);
    }
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

void RESerial::openOutput()
{
    char bufDir[127];
    char bufSubDir[127];
    QStringToCStr(workDirectory, bufDir, 127);
    QStringToCStr(subDirectory, bufSubDir, 127);
    openFiles = open_folder(bufDir, bufSubDir);
    if(openFiles.dataFile == 0 || openFiles.settingsFile == 0 || openFiles.mainLog == 0)
    {
        throw tr("RE couldn't open output files!\n");
    }
}

void RESerial::closeOutput()
{
    if(openFiles.dataFile!=0)fclose(openFiles.dataFile);
    if(openFiles.mainLog!=0)fclose(openFiles.mainLog);
    if(openFiles.settingsFile!=0)fclose(openFiles.settingsFile);
}

void RESerial::writeExpInfo(const Info_Settings ist)
{
    fprintf(openFiles.settingsFile, "#Food: %u\n", Settings.food);
    fprintf(openFiles.settingsFile, "#Manual: %c\n", Settings.manual ? 'Y' : 'N');
    fprintf(openFiles.settingsFile, "#Max delay: %u\n", Settings.max_delay);
    fprintf(openFiles.settingsFile, "#Min delay: %u\n", Settings.min_delay);
    fprintf(openFiles.settingsFile, "#Press interval: %u\n", Settings.press_interval);
    fprintf(openFiles.settingsFile, "#Sound length: %u\n\n", Settings.sound_length);

    fprintf(openFiles.settingsFile, "#Actor: %s\n", ist.actorName);
    fprintf(openFiles.settingsFile, "#Actor weight: %u\n", ist.actorWeight);
    fprintf(openFiles.settingsFile, "#Spectator: %s\n", ist.spectatorName);
    fprintf(openFiles.settingsFile, "#Spectators weight: %u\n", ist.spectatorWeight);
    fprintf(openFiles.settingsFile, "#Worker's name: %s\n", ist.workerName);
    fprintf(openFiles.settingsFile, "#Date: %s\n", ist.date);
    fprintf(openFiles.settingsFile, "#Mode: %c\n", ist.learning ? 'L' : 'E');
}

void RESerial::setWorkDir(const QString &wd, const QString &sd)
{
    workDirectory = wd;
    subDirectory = sd;
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
    Settings = st;
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
    while(canReadLine())
    {
        char buf[64];
        uint16_t len = readLine(buf, 64);
        qDebug() << buf;
        fputs(buf, openFiles.mainLog);
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
            serialTimer->stop();
            State = AwaitEvent;
            //redact and add writing to .log
            return;
        }
        if(proc_data.typ == DTComment)
        {
            retimeToStr(GetTimeStamp(), 0, buf);
            newMessage(tr(buf) + " " + ToStr(proc_data));
            return;
        }
        retimeToStr(GetTimeStamp(), 0, buf);
        fputs(buf, openFiles.dataFile);
        fputc(' ', openFiles.dataFile);
        fputs(ToStr(proc_data), openFiles.dataFile);
        fputc('\n', openFiles.dataFile);
        newMessage(tr(buf) + " " + ToStr(proc_data) + "\n");
    }
}

void RESerial::processAE(void)
//Proceesing in await event or command from device.
//if error in message found then error will send to device
//awaiting resending for the previous message.
//if all correct state doesn't change.
{
    while(canReadLine())
    {
        char buf[64];
        uint16_t len = readLine(buf, 64);
        qDebug() << buf;
        fputs(buf, openFiles.mainLog);
        __re_abstract_data proc_data = process_message(buf,len);
        if(proc_data.typ == DTError)
        {
            emit statusChange(tr(ToStr(proc_data)));
            //redact and add writing to .log
            return;
        }
        if(proc_data.typ == DTComment)
        {
            retimeToStr(GetTimeStamp(), 0, buf);
            newMessage(tr(buf) + " " + ToStr(proc_data));
            return;
        }
        retimeToStr(GetTimeStamp(), 0, buf);
        fputs(buf, openFiles.dataFile);
        fputc(' ', openFiles.dataFile);
        fputs(ToStr(proc_data), openFiles.dataFile);
        fputc('\n', openFiles.dataFile);
        newMessage(tr(buf) + " " + ToStr(proc_data) + "\n");
    }
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
