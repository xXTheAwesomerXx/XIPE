#include "uccxmigrationtab.h"
#include "ui_uccxmigrationtab.h"

UCCXMigrationTab::UCCXMigrationTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UCCXMigrationTab)
{
    ui->setupUi(this);
    tableWidget = new QTableWidget;
    ui->scrollArea->setWidget(tableWidget);
}

UCCXMigrationTab::~UCCXMigrationTab()
{
    delete ui;
}
