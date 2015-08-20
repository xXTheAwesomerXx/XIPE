#include "clustertab.h"
#include "ui_clustertab.h"

ClusterTab::ClusterTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterTab)
{
    ui->setupUi(this);
    listWidget = new QListWidget;
    listWidget2 = new QListWidget;
    ui->scrollArea->setWidget(listWidget);
    ui->scrollArea_2->setWidget(listWidget2);
}

ClusterTab::~ClusterTab()
{
    delete ui;
}
