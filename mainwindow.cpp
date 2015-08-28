#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "endusersclustermngmtmainwindow.h"
#include "uccxmigrationmainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QPushButton {background: #3498db;border-style: outset;border-width: 2px;border-radius: 10px;font-family: Arial;color: #ffffff;font-size: 20px;padding: 10px 20px 10px 20px;text-decoration: none;}");
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
