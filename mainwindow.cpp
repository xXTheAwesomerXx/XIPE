#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "endusersclustermngmtmainwindow.h"
#include "uccxmigrationmainwindow.h"
#include "cmuserdevicemanagermainwindow.h"
#include "variables.h" //Include this here so variables are initialized
#include "loginterfacedialog.h"
#include "loginterfacedialog.h"
#include "cmuserdevicemanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QPushButton {background: #3498db;border-style: outset;border-width: 2px;border-radius: 10px;font-family: Arial;color: #ffffff;font-size: 20px;padding: 10px 20px 10px 20px;text-decoration: none;}");
    LogInterfaceDialog * logInterface;
    if (Variables::logInterfaceDone == false) {
        logInterface = new LogInterfaceDialog();
        Variables::logInterface = logInterface;
        Variables::logInterfaceDone = true;
    }
    if (Variables::logInterfaceShowing == true) {
        if (Variables::logInterface->isHidden()) {
            Variables::logInterface->show();
            Variables::logInterfaceShowing = true;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnEndusersClusterMngmt_clicked()
{
    EndusersClusterMngmtMainWindow * window = new EndusersClusterMngmtMainWindow();
    window->show();
    this->hide();
//    EndusersClusterMngmt managementWindow;
//    managementWindow.setModal(true);
//    managementWindow.exec();
}

void MainWindow::on_btnUCCXMigration_clicked()
{
    this->hide();
    UCCXMigrationMainWindow * uccxmainwindow = new UCCXMigrationMainWindow();
    uccxmainwindow->show();
//    UCCXMainWindow uccxWindow;
//    uccxWindow.setModal(true);
//    uccxWindow.exec();
}

void MainWindow::on_btnEnduserDeviceMngr_clicked()
{
    CMUserDeviceManagerMainWindow * window = new CMUserDeviceManagerMainWindow();
    window->show();
    this->hide();
}
