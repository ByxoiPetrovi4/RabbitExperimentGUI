#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialDialog = new SerialDiag(this);
    rabbitDialog = new RabbitDiag(this);
    serialPort = new QSerialPort(this);

    connected = false;

    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showStatusMessage(const QString &str)
{
    ui->connectStatus->setText(str);
}

void MainWindow::readData()
{
    const QString str(serialPort->readAll());
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(str);
}

void MainWindow::on_actionSerial_settings_triggered()
{
    serialDialog->show();
}


void MainWindow::on_actionRecord_settings_triggered()
{
    rabbitDialog->show();

}


void MainWindow::on_connectButton_clicked()
{
    if(connected)
    {
        if(serialPort->disconnect())
        {
            showStatusMessage(tr("Disconnected"));
            connected = false;
            ui->connectButton->setText(tr("Connect"));
            return;
        }
        QMessageBox::critical(this, tr("Error"), serialPort->errorString());
        showStatusMessage(tr("Serial ERROR!"));
    }
    const SerialDiag::Settings p = serialDialog->settings();
    serialPort->setPortName(p.name);
    serialPort->setBaudRate(p.baudRate);
    serialPort->setDataBits(p.dataBits);
    serialPort->setParity(p.parity);
    serialPort->setStopBits(p.stopBits);
    serialPort->setFlowControl(p.flowControl);
    if (serialPort->open(QIODevice::ReadWrite)) {
        connected = true;
        ui->connectButton->setText(tr("Disconnect"));
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serialPort->errorString());

        showStatusMessage(tr("Open error"));
    }
}

