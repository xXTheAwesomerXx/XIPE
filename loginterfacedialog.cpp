#include "loginterfacedialog.h"
#include "ui_loginterfacedialog.h"
#include "variables.h"
#include "qdebug.h"
#include "QScrollBar"

LogInterfaceDialog::LogInterfaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogInterfaceDialog)
{
    ui->setupUi(this);
    Variables::logScrollBar = ui->textBrowser->verticalScrollBar();
}

LogInterfaceDialog::~LogInterfaceDialog()
{
    delete ui;
}

void LogInterfaceDialog::closeEvent(QCloseEvent * event) {
    Variables::logInterface->hide();
    Variables::logInterfaceShowing = false;
}

void LogInterfaceDialog::setLogText(QString text) {
    ui->textBrowser->setText(text);
}

void LogInterfaceDialog::on_textBrowser_textChanged()
{
    qDebug() << "Text changed...do something?!";
    QScrollBar * sb = Variables::logScrollBar;
    sb->setValue(sb->maximum());
}
