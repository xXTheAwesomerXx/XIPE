#include "findusersmainwindow.h"
#include "ui_findusersmainwindow.h"

FindUsersMainWindow::FindUsersMainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindUsersMainWindow)
{
    ui->setupUi(this);
}

FindUsersMainWindow::~FindUsersMainWindow()
{
    delete ui;
}
