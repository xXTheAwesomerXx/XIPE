#include "cmuserdevicemanagermainwindow.h"
#include "ui_cmuserdevicemanagermainwindow.h"
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslSocket>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QMessageBox>
#include "variables.h"
#include "mainwindow.h"

using namespace Variables;
QString base64_encode(QString string);
QString base64_decode(QString string);
void appendToFile(QString text, QString filePath, QString fileName);

CMUserDeviceManagerMainWindow::CMUserDeviceManagerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMUserDeviceManagerMainWindow)
{
    ui->setupUi(this);
    QStringList uccxVersions;
    uccxVersions << "8.x" << "9.x" << "10.x" << "11.x";
    ui->comboboxVersion->addItems(uccxVersions);
    ui->listWidgetConnections->setSelectionMode(QAbstractItemView::SingleSelection);
    QFile inputFile(QDir::homePath() + "/XIPE/Cluster\ Mngmt/conn.conf");
    QDir appDir;
    appendToFile("Started Call Manager Enduser & Device Utility", QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
    if (appDir.exists(QDir::homePath() + "/XIPE/Cluster\ Mngmt")) {
        if (inputFile.open(QIODevice::ReadWrite)) {
            QTextStream in(&inputFile);
                   while (!in.atEnd())
                   {
                      QString line = base64_decode(in.readLine());
                      QDomDocument doc;
                      doc.setContent(line.toLocal8Bit());
                          QDomNodeList rates = doc.elementsByTagName("cluster");
                          for (int i = 0; i < rates.size(); i++) {
                              QDomNode n = rates.item(i);
                              QDomElement clustername = n.firstChildElement("name");
                              QDomElement hostname = n.firstChildElement("host");
                              QDomElement unpassword = n.firstChildElement("unpwd");
                              QDomElement version = n.firstChildElement("version");
                              Variables::clusterNames.append(clustername.text());
                              Variables::hostNames.append(hostname.text());
                              Variables::usernamePasswords.append(unpassword.text());
                              Variables::clusterVersions.append(version.text());
                              QListWidgetItem *cluster = new QListWidgetItem(clustername.text());
                              cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false);
                              cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                              cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                              ui->listWidgetConnections->addItem(cluster);
                          }
                   }
                   inputFile.close();
        }
    } else {
        if (appDir.mkpath(QDir::homePath() + "/XIPE/Cluster\ Mngmt")) {
            if (inputFile.open(QIODevice::ReadWrite)) {
                QTextStream in(&inputFile);
                       while (!in.atEnd())
                       {
                          QString line = in.readLine();
                       }
                       inputFile.close();
            }
        }
    }
}

CMUserDeviceManagerMainWindow::~CMUserDeviceManagerMainWindow()
{
    delete ui;
}
