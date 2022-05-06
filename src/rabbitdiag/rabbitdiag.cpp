#include "rabbitdiag.h"
#include "ui_rabbitdiag.h"

RabbitDiag::RabbitDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RabbitDiag)
{
    ui->setupUi(this);
}

RabbitDiag::~RabbitDiag()
{
    delete ui;
}
