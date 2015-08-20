#include "findusersmngmt.h"
#include "ui_findusersmngmt.h"

FindUsersMngmt::FindUsersMngmt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindUsersMngmt)
{
    ui->setupUi(this);
}

FindUsersMngmt::~FindUsersMngmt()
{
    delete ui;
}
