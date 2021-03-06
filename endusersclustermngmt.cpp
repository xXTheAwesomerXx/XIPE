#include "endusersclustermngmt.h"
#include "ui_endusersclustermngmt.h"
#include "endusersclustermngmttabs.h"
#include <QDebug>
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
#include "progressdialog.h"
using namespace Variables;
EndusersClusterMngmt::EndusersClusterMngmt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndusersClusterMngmt)
{
    ui->setupUi(this);//set up load/makefile here
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    QFile inputFile(QDir::homePath() + "/XIPE/ClusterMngmt/test.txt");
    QDir appDir;
    if (appDir.exists(QDir::homePath() + "/XIPE/ClusterMngmt")) {
        qDebug() << "Path is here";
        if (inputFile.open(QIODevice::ReadWrite)) {
            QTextStream in(&inputFile);
                   while (!in.atEnd())
                   {
                      QString line = in.readLine();
                      qDebug() << line;
                      QDomDocument doc;
                      doc.setContent(line.toLocal8Bit());
                          QDomNodeList rates = doc.elementsByTagName("cluster");
                          for (int i = 0; i < rates.size(); i++) {
                              QDomNode n = rates.item(i);
                              QDomElement clustername = n.firstChildElement("name");
                              QDomElement hostname = n.firstChildElement("host");
                              QDomElement unpassword = n.firstChildElement("unpwd");
                              Variables::clusterNames.append(clustername.text());
                              Variables::hostNames.append(hostname.text());
                              Variables::usernamePasswords.append(unpassword.text());
                              QListWidgetItem *cluster = new QListWidgetItem(clustername.text());
                              cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
                              cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
                              cluster->setCheckState(Qt::Unchecked);
                              cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
                              ui->listWidget->addItem(cluster);
                          }
                   }
                   inputFile.close();
        }
    } else {
        if (appDir.mkpath(QDir::homePath() + "/XIPE/ClusterMngmt")) {
            qDebug() << "We made the path";
            if (inputFile.open(QIODevice::ReadWrite)) {
                QTextStream in(&inputFile);
                       while (!in.atEnd())
                       {
                          QString line = in.readLine();
                          qDebug() << line;
                       }
                       inputFile.close();
            }
        }
    }
}

EndusersClusterMngmt::~EndusersClusterMngmt()
{
    delete ui;
}

void EndusersClusterMngmt::on_pushButtonProceedToMngmt_clicked()
{
    int numOfConns = 0;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
            Variables::clusterNamesF.append(Variables::clusterNames[i]);
            Variables::hostNamesF.append(Variables::hostNames[i]);
            Variables::usernamePasswordsF.append(Variables::usernamePasswords[i]);
            numOfConns++;
        }
    }
    if (numOfConns > 0) {
        this->hide();
        EndusersClusterMngmtTabs tabbedWindow;
        tabbedWindow.setModal(true);
        tabbedWindow.exec();
    } else {
        QMessageBox::critical(this, "CMClusters - Error", "No clusters were selected! Please select at least one cluster and try again. \n\n Before being able to select a cluster, you must first test the connection!");
    }
}

bool EndusersClusterMngmt::addConnection(QString hostname, QString usernamepassword, QLabel * label, QPushButton * button) {
    label->setStyleSheet("color: rgb(0, 0, 0);");
    label->setText("Connecting to " + hostname.toLocal8Bit() + " please wait!");
    button->setEnabled(false);
    QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
    jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser from enduser where enduser.firstname LIKE 'A%'</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    QByteArray postDataSize = QByteArray::number(jsonString.size());
    QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
    QNetworkRequest request(req);

    request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLQuery\"");
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
    if ( !statusCode.isValid() ) {
        label->setText("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray());
        label->setStyleSheet("color: rgb(170, 0, 0);");
        button->setEnabled(true);
        return false;
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        button->setEnabled(true);
        label->setText("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray());
        label->setStyleSheet("color: rgb(170, 0, 0);");
        return false;
    } else {
        button->setEnabled(true);
        label->setText("Successfully connected to " + hostname.toLocal8Bit() + "!");
        label->setStyleSheet("color: rgb(0, 170, 0);");
        return true;
    }
    button->setEnabled(true);
    return false;
}

void EndusersClusterMngmt::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void EndusersClusterMngmt::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

//void EndusersClusterMngmt::on_btnAddCluster_clicked()
//{
//    if ((!ui->lineEditHostname->text().isEmpty()) && (!ui->lineEditUsername->text().isEmpty()) && (!ui->lineEditPassword->text().isEmpty())) {
//        if (!ui->lineEditClustername->text().isEmpty()) {
//            QList<QListWidgetItem *> items = ui->listWidget->findItems(ui->lineEditClustername->text(), Qt::MatchExactly);
//            if (items.size() > 0) {
//                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
//            } else {
//                if (ui->checkBoxConnect->isChecked()) {
//                    if (addConnection(ui->lineEditHostname->text(), base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()), ui->labelConnectionsInfoMessage, ui->btnAddCluster)) {
//                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClustername->text(), ui->listWidget);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                        cluster->setCheckState(Qt::Checked);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                        Variables::clusterNames.append(ui->lineEditClustername->text());
//                        Variables::hostNames.append(ui->lineEditHostname->text());
//                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                        QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                        if ( file.open(QIODevice::Append) )
//                        {
//                            QTextStream stream( &file );
//                            stream << QString("<cluster><name>" + ui->lineEditClustername->text().toLocal8Bit() + "</name><host>" + ui->lineEditHostname->text().toLocal8Bit() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()).toLocal8Bit() + "</unpwd></cluster>") << endl;
//                        } else {
//                            qDebug() << "Why couldn't we wright?";
//                        }

//                        ui->lineEditClustername->setText("");
//                        ui->lineEditHostname->setText("");
//                        ui->lineEditPassword->setText("");
//                        ui->lineEditUsername->setText("");
//                    } else {
//                        // Maybe popup here? because we weren't able to connect
//                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClustername->text(), ui->listWidget);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                        cluster->setSelected(true);
//                        cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                        Variables::clusterNames.append(ui->lineEditClustername->text());
//                        Variables::hostNames.append(ui->lineEditHostname->text());
//                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                        QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                        if ( file.open(QIODevice::Append) )
//                        {
//                            QTextStream stream( &file );
//                            stream << QString("<cluster><name>" + ui->lineEditClustername->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd></cluster>") << endl;
//                        }

//                        ui->lineEditClustername->setText("");
//                        ui->lineEditHostname->setText("");
//                        ui->lineEditPassword->setText("");
//                        ui->lineEditUsername->setText("");
//                    }
//                } else {
//                    // Just add the item
//                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditClustername->text(), ui->listWidget);
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                    cluster->setSelected(true);
//                    cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                    Variables::clusterNames.append(ui->lineEditClustername->text());
//                    Variables::hostNames.append(ui->lineEditHostname->text());
//                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                    QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                    if ( file.open(QIODevice::Append) )
//                    {
//                        QTextStream stream( &file );
//                        stream << QString("<cluster><name>" + ui->lineEditClustername->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd></cluster>") << endl;
//                    }

//                    ui->lineEditClustername->setText("");
//                    ui->lineEditHostname->setText("");
//                    ui->lineEditPassword->setText("");
//                    ui->lineEditUsername->setText("");
//                }
//            }
//        } else {
//            QList<QListWidgetItem *> items = ui->listWidget->findItems(ui->lineEditHostname->text(), Qt::MatchExactly);
//            if (items.size() > 0) {
//                QMessageBox::critical(this, "CMClusters - Error", "A cluster was already found with that name! *You can leave the cluster name field blank and the added cluster will have a name of the clusters Hostname/IP by default.");
//            } else {
//                if (ui->checkBoxConnect->isChecked()) {
//                    if (addConnection(ui->lineEditHostname->text(), base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()), ui->labelConnectionsInfoMessage, ui->btnAddCluster)) {
//                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listWidget);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                        cluster->setCheckState(Qt::Checked);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                        Variables::clusterNames.append(ui->lineEditHostname->text());
//                        Variables::hostNames.append(ui->lineEditHostname->text());
//                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                        QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                        if ( file.open(QIODevice::Append) )
//                        {
//                            QTextStream stream( &file );
//                            stream << QString("<cluster><name>" + ui->lineEditHostname->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd></cluster>") << endl;
//                        }

//                        ui->lineEditClustername->setText("");
//                        ui->lineEditHostname->setText("");
//                        ui->lineEditPassword->setText("");
//                        ui->lineEditUsername->setText("");
//                    } else {
//                        // Maybe popup here? because we weren't able to connect
//                        QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listWidget);
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                        cluster->setSelected(true);
//                        cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
//                        cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                        Variables::clusterNames.append(ui->lineEditHostname->text());
//                        Variables::hostNames.append(ui->lineEditHostname->text());
//                        Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                        QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                        if ( file.open(QIODevice::Append) )
//                        {
//                            QTextStream stream( &file );
//                            stream << QString("<cluster><name>" + ui->lineEditHostname->text().toLocal8Bit() + "</name><host>" + ui->lineEditHostname->text().toLocal8Bit() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()).toLocal8Bit() + "</unpwd></cluster>") << endl;
//                        }

//                        ui->lineEditClustername->setText("");
//                        ui->lineEditHostname->setText("");
//                        ui->lineEditPassword->setText("");
//                        ui->lineEditUsername->setText("");
//                    }
//                } else {
//                    // Just add the item
//                    QListWidgetItem* cluster = new QListWidgetItem(ui->lineEditHostname->text(), ui->listWidget);
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsUserCheckable = false); //Let's make it so
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsSelectable);
//                    cluster->setSelected(true);
//                    cluster->setCheckState(Qt::Unchecked); //XXX: Figure out how to explicitly
//                    cluster->setFlags(cluster->flags() | Qt::ItemIsEnabled);
//                    Variables::clusterNames.append(ui->lineEditHostname->text());
//                    Variables::hostNames.append(ui->lineEditHostname->text());
//                    Variables::usernamePasswords.append(base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()));

//                    QFile file( QDir::homePath() + "/XIPE/ClusterMngmt/test.txt" );
//                    if ( file.open(QIODevice::Append) )
//                    {
//                        QTextStream stream( &file );
//                        stream << QString("<cluster><name>" + ui->lineEditHostname->text() + "</name><host>" + ui->lineEditHostname->text() + "</host><unpwd>" + base64_encode(ui->lineEditUsername->text() + QString::fromStdString(":") + ui->lineEditPassword->text()) + "</unpwd></cluster>") << endl;
//                    }

//                    ui->lineEditClustername->setText("");
//                    ui->lineEditHostname->setText("");
//                    ui->lineEditPassword->setText("");
//                    ui->lineEditUsername->setText("");
//                }
//            }
//        }
//    } else {
//        QMessageBox::critical(this, "CMClusters - Error", "One of fields Hostname, Username, or Password has no value!");
//    }
//}

void EndusersClusterMngmt::on_pushButtonTestConnections_clicked()
{
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->isSelected())
            if (addConnection(Variables::hostNames[i], Variables::usernamePasswords[i], ui->labelConnectionsInfoMessage, ui->btnAddCluster))
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}
