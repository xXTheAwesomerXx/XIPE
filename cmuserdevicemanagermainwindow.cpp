#include "cmuserdevicemanagermainwindow.h"
#include "ui_cmuserdevicemanagermainwindow.h"

CMUserDeviceManagerMainWindow::CMUserDeviceManagerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMUserDeviceManagerMainWindow)
{
    ui->setupUi(this);
}

CMUserDeviceManagerMainWindow::~CMUserDeviceManagerMainWindow()
{
    delete ui;
}
