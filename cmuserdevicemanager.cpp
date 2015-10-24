#include "cmuserdevicemanager.h"
#include "ui_cmuserdevicemanager.h"
#include "cmuserdevicemanagermainwindow.h"
#include "variables.h"

using namespace Variables;
CMUserDeviceManager::CMUserDeviceManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMUserDeviceManager)
{
    ui->setupUi(this);
    if (Variables::logInterfaceShowing == true) {
        ui->actionLog_Interface->setChecked(true);
    }
}

CMUserDeviceManager::~CMUserDeviceManager()
{
    delete ui;
}

void CMUserDeviceManager::on_actionBack_to_Cluster_List_triggered()
{
    CMUserDeviceManagerMainWindow * backwindow = new CMUserDeviceManagerMainWindow();
    backwindow->show();
    this->close();
}

void CMUserDeviceManager::on_actionExit_triggered()
{
    this->close();
}

void CMUserDeviceManager::on_actionLog_Interface_triggered()
{
    if (Variables::logInterfaceShowing == true) {
        Variables::logInterfaceShowing = false;
        Variables::logInterface->hide();
    } else {
        Variables::logInterfaceShowing = true;
        Variables::logInterface->show();
    }
}
