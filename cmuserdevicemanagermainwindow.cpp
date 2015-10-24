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
#include "cmuserdevicemanager.h"

using namespace Variables;
QString base64_encode(QString string);
QString base64_decode(QString string);
void appendToFile(QString text, QString filePath, QString fileName);

CMUserDeviceManagerMainWindow::CMUserDeviceManagerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMUserDeviceManagerMainWindow)
{
    ui->setupUi(this);
    Variables::clusterNames.clear();
    Variables::clusterNamesF.clear();
    Variables::clusterVersions.clear();
    Variables::clusterVersionsF.clear();
    Variables::hostNames.clear();
    Variables::hostNamesF.clear();
    Variables::usernamePasswords.clear();
    Variables::usernamePasswordsF.clear();
    if (Variables::logInterfaceShowing == true) {
        ui->actionLog_Interface->setChecked(true);
    }
    QStringList uccxVersions;
    uccxVersions << "9.x" << "10.x" << "11.x";
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
                              QListWidgetItem *cluster = new QListWidgetItem(clustername.text() + " (" + hostname.text() + ") | " + version.text());
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

void CMUserDeviceManagerMainWindow::on_actionLog_Interface_triggered()
{
    if (Variables::logInterfaceShowing == true) {
        Variables::logInterfaceShowing = false;
        Variables::logInterface->hide();
    } else {
        Variables::logInterfaceShowing = true;
        Variables::logInterface->show();
    }
}

void CMUserDeviceManagerMainWindow::on_actionRemove_Cluster_triggered()
{
    for (int i = ui->listWidgetConnections->count(); i --> 0;) {
        if (ui->listWidgetConnections->item(i)->isSelected()) {
        QFile f(QDir::homePath() + "/XIPE/Cluster\ Mngmt/conn.conf");
        if(f.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QString s;
            QTextStream t(&f);
            while(!t.atEnd())
            {
                QString line = t.readLine();
                if(!line.contains(base64_encode("<cluster><name>" + Variables::clusterNames[i].toLocal8Bit() + "</name><host>" + Variables::hostNames[i].toLocal8Bit() + "</host><unpwd>" + Variables::usernamePasswords[i].toLocal8Bit() + "</unpwd><version>" + Variables::clusterVersions[i].toLocal8Bit() + "</version></cluster>"))) {
                    s.append(line + "\n");
                } else {
                    ui->statusbar->showMessage("Removed cluster: " + Variables::clusterNames[i] + " from configuration!");
                }
            }
            f.resize(0);
            t << s;
            f.close();
        }
        if (ui->listWidgetConnections->item(i)->checkState() == Qt::Checked) {
            Variables::clusterNamesF.removeOne(Variables::clusterNames[i]);
            Variables::hostNamesF.removeOne(Variables::hostNames[i]);
            Variables::usernamePasswordsF.removeOne(Variables::usernamePasswords[i]);
            Variables::clusterVersionsF.removeOne(Variables::clusterVersions[i]);
        }
        Variables::clusterNames.remove(i);
        Variables::hostNames.remove(i);
        Variables::usernamePasswords.remove(i);
        Variables::clusterVersions.remove(i);
        delete ui->listWidgetConnections->item(i);
        }
    }
}

bool CMUserDeviceManagerMainWindow::addConnection(QString hostname, QString usernamepassword, QString version, QStatusBar * statusbar, QPushButton * button) {
    statusbar->showMessage("Connecting to " + hostname.toLocal8Bit() + " please wait!");
    button->setEnabled(false);
    appendToFile("Connecting to " + hostname.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
    QByteArray jsonString;
    if (version != "8.x") {
        jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser from enduser where enduser.firstname LIKE 'A%'</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    } else {
        jsonString = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        jsonString += "<SOAP-ENV:Body> <axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\">";
        jsonString += "<sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser from enduser where enduser.firstname LIKE 'A%'</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>";
    }
    QByteArray postDataSize = QByteArray::number(jsonString.size());
    QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
    QNetworkRequest request(req);

    if (version == "8.x") {
        request.setRawHeader("Host", hostname.toLocal8Bit() + ":8443");
        request.setRawHeader("Accept", "text/*");
        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=7.0 executeSQLQuery\"");
    } else {
        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLQuery\"");

    }
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    request.setRawHeader("Content-Type", "text/xml");
    request.setRawHeader("Content-Length", postDataSize);

    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.post(request, jsonString);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    int status = statusCode.toInt();

    if ( !statusCode.isValid() ) {
        statusbar->showMessage("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + QString::number(status));
        appendToFile("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + QString::number(status), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
        button->setEnabled(true);
        return false;
    }



    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        button->setEnabled(true);
        statusbar->showMessage("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + QString::number(status));
        appendToFile("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + QString::number(status), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
        return false;
    } else {
        button->setEnabled(true);
        statusbar->showMessage("Successfully connected to " + hostname.toLocal8Bit() + "!");
        appendToFile("Successfully connected to " + hostname.toLocal8Bit() + "!", QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
        return true;
    }
    button->setEnabled(true);
    return false;
}

void CMUserDeviceManagerMainWindow::setStatusBarMessage(QString text) {
    ui->statusbar->showMessage(text);
}

void CMUserDeviceManagerMainWindow::on_actionBack_to_Main_Window_triggered()
{
    MainWindow * window = new MainWindow();
    window->show();
    this->close();
}

void CMUserDeviceManagerMainWindow::on_actionExit_triggered()
{
    this->close();
}

void CMUserDeviceManagerMainWindow::on_pushButtonSave_clicked()
{
    if ((!ui->lineEditHostname->text().isEmpty()) && (!ui->lineEditUsername->text().isEmpty()) && (!ui->lineEditPassword->text().isEmpty())) {
        if (!ui->lineEditClustername->text().isEmpty()) {
            QList<QListWidgetItem *> items = ui->listWidgetConnections->findItems(ui->lineEditClustername->text() + " (" + ui->lineEditHostname->text() + ") | " + ui->comboboxVersion->currentText(), Qt::MatchExactly);
            if (items.size() > 0) {
                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
            } else {
                    // Just add the item
                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClustername->text() + " (" + ui->lineEditHostname->text() + ") | " + ui->comboboxVersion->currentText(), ui->listWidgetConnections);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                    cluster->setSelected(true);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                    Variables::clusterNames.append(ui->lineEditClustername->text());
                    Variables::hostNames.append(ui->lineEditHostname->text());
                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    Variables::clusterVersions.append(ui->comboboxVersion->currentText());
                    QFile file( QDir::homePath() + "/XIPE/Cluster\ Mngmt/conn.conf" );
                    if ( file.open(QIODevice::Append) )
                    {
                        QTextStream stream( &file );
                        stream << base64_encode(QString("<cluster><name>" + ui->lineEditClustername->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd><version>" + ui->comboboxVersion->currentText() + "</version></cluster>")) << endl;
                        file.close();
                    }

                    ui->lineEditClustername->setText("");
                    ui->lineEditHostname->setText("");
                    ui->lineEditPassword->setText("");
                    ui->lineEditUsername->setText("");
                    ui->comboboxVersion->setCurrentIndex(0);
            }
        } else {
            QList<QListWidgetItem *> items = ui->listWidgetConnections->findItems(ui->lineEditHostname->text() + " (" + ui->lineEditHostname->text() + ") | " + ui->comboboxVersion->currentText(), Qt::MatchExactly);
            if (items.size() > 0) {
                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
            } else {// Just add the item
                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text() + " (" + ui->lineEditHostname->text() + ") | " + ui->comboboxVersion->currentText(), ui->listWidgetConnections);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                    cluster->setSelected(true);
                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                    Variables::clusterNames.append(ui->lineEditHostname->text());
                    Variables::hostNames.append(ui->lineEditHostname->text());
                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));
                    Variables::clusterVersions.append(ui->comboboxVersion->currentText());
                    QFile file( QDir::homePath() + "/XIPE/Cluster\ Mngmt/conn.conf" );
                    if ( file.open(QIODevice::Append) )
                    {
                        QTextStream stream( &file );
                        stream << base64_encode(QString("<cluster><name>" + ui->lineEditHostname->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd><version>" + ui->comboboxVersion->currentText() + "</version></cluster>")) << endl;
                        file.close();
                    }

                    ui->lineEditClustername->setText("");
                    ui->lineEditHostname->setText("");
                    ui->lineEditPassword->setText("");
                    ui->lineEditUsername->setText("");
                    ui->comboboxVersion->setCurrentIndex(0);
                }
        }
    } else {
        QMessageBox::critical(this, "CMClusters - Error", "One of fields Hostname, Username, or Password has no value!");
    }
}

void CMUserDeviceManagerMainWindow::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void CMUserDeviceManagerMainWindow::onError(QNetworkReply::NetworkError rep) {
    appendToFile("Encountered an error while attempting to establish a secure connection! Retrying...", QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
}

void CMUserDeviceManagerMainWindow::on_pushButtonConnect_clicked()
{
    for (int i = 0; i < ui->listWidgetConnections->count(); i++) {
        if (ui->listWidgetConnections->item(i)->isSelected()) {
            if (addConnection(Variables::hostNames[i], Variables::usernamePasswords[i], Variables::clusterVersions[i], ui->statusbar, ui->pushButtonConnect)) {
                Variables::clusterNamesF.append(Variables::clusterNames[i]);
                Variables::clusterVersionsF.append(Variables::clusterVersions[i]);
                Variables::hostNamesF.append(Variables::hostNames[i]);
                Variables::usernamePasswordsF.append(Variables::usernamePasswords[i]);
                CMUserDeviceManager * newwindow = new CMUserDeviceManager();
                newwindow->show();
                this->close();
            }
        }
    }
}
