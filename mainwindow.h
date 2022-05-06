#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialdiag.h"
#include "rabbitdiag.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSerial_settings_triggered();

    void on_actionRecord_settings_triggered();

private:
    Ui::MainWindow *ui;
    SerialDiag*     serialDialog;
    RabbitDiag*     rabbitDialog;
};
#endif // MAINWINDOW_H
