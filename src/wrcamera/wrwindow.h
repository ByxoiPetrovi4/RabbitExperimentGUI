#ifndef WRWINDOW_H
#define WRWINDOW_H

#include <QWidget>
#include "wrcamera.h"

namespace Ui {
class wrwindow;
}

class wrwindow : public QWidget
{
    Q_OBJECT

public:
    explicit wrwindow(QWidget *parent = nullptr);
    ~wrwindow();
private slots:
    void on_writeButton_clicked();

    void on_displayCBox_stateChanged(int arg1);

private:
    Ui::wrwindow *ui;
    CameraWriter camwr;
};

#endif // WRWINDOW_H
