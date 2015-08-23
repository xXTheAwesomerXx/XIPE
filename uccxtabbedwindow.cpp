#include "uccxtabbedwindow.h"
#include "ui_uccxtabbedwindow.h"
#include "uccxmigrationtab.h"
#include <QTableWidget>
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
using namespace Variables;
QVector<QTableWidget*> myTableWidgets;
QVector<QString> teamRefLinks, appRefLinks, skillRefLinks, rgRefLinks;
UCCXTabbedWindow::UCCXTabbedWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UCCXTabbedWindow)
{
    ui->setupUi(this);
        UCCXMigrationTab * skillTab = new UCCXMigrationTab();
        myTableWidgets.append(skillTab->tableWidget);
        ui->tabWidget->addTab(skillTab, "Skills");
        UCCXMigrationTab * rgTab = new UCCXMigrationTab();
        myTableWidgets.append(rgTab->tableWidget);
        ui->tabWidget->addTab(rgTab, "Resource Groups");
        UCCXMigrationTab * appTab = new UCCXMigrationTab();
        myTableWidgets.append(appTab->tableWidget);
        ui->tabWidget->addTab(appTab, "Applications");
        UCCXMigrationTab * csqTab = new UCCXMigrationTab();
        myTableWidgets.append(csqTab->tableWidget);
        ui->tabWidget->addTab(csqTab, "CSQs");
        UCCXMigrationTab * teamTab = new UCCXMigrationTab();
        myTableWidgets.append(teamTab->tableWidget);
        ui->tabWidget->addTab(teamTab, "Teams");
        UCCXMigrationTab * triggerTab = new UCCXMigrationTab();
        myTableWidgets.append(triggerTab->tableWidget);
        ui->tabWidget->addTab(triggerTab, "Triggers");
        QStringList skillHeaders, rgHeaders, appHeaders, csqHeaders, teamHeaders, triggerHeaders;
        teamHeaders << "Name" << "ID" << "Primary Supervisor" << "Secondary Supervisor" << "Resources" << "CSQs";
        myTableWidgets[4]->setColumnCount(6);
        myTableWidgets[4]->setHorizontalHeaderLabels(teamHeaders);
        appHeaders << "Name" << "ID" << "Type" << "Description" << "Enabled";
        myTableWidgets[2]->setColumnCount(5);
        myTableWidgets[2]->setHorizontalHeaderLabels(appHeaders);
        skillHeaders << "Name" << "ID";
        myTableWidgets[0]->setColumnCount(2);
        myTableWidgets[0]->setHorizontalHeaderLabels(skillHeaders);
        rgHeaders << "Name" << "ID";
        myTableWidgets[1]->setColumnCount(2);
        myTableWidgets[1]->setHorizontalHeaderLabels(rgHeaders);
}

UCCXTabbedWindow::~UCCXTabbedWindow()
{
    delete ui;
}

void UCCXTabbedWindow::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void UCCXTabbedWindow::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

bool UCCXTabbedWindow::getAllTeamData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting team data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/team");
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    } else {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList teams = doc.elementsByTagName("team");
        for (int i = 0; i < teams.size(); i++) {
            QDomNode n = teams.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement teamId = n.firstChildElement("teamId");
            QDomElement teamname = n.firstChildElement("teamname");
            teamRefLinks.append(self.text());
            qDebug() << teamRefLinks[i];
        }
        return true;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedTeamData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting team data";
    QUrl req(refURL);
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get team data";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get team data";
    } else {
        qDebug() << "Connected, lets get the data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList teams = doc.elementsByTagName("team");
        for (int i = 0; i < teams.size(); i++) {
            QDomNode n = teams.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement teamId = n.firstChildElement("teamId");
            QDomElement teamname = n.firstChildElement("teamname");
            QDomElement primarySup = n.firstChildElement("primarySupervisor");
            QDomElement primarySupRef = primarySup.firstChildElement("refURL");
            myTableWidgets[4]->insertRow(myTableWidgets[4]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[4]->rowCount();
            int row = myTableWidgets[4]->rowCount() - 1;
            myTableWidgets[4]->setItem(row, 0, new QTableWidgetItem(teamname.text()));
            myTableWidgets[4]->setItem(row, 1, new QTableWidgetItem(teamId.text()));
            if (primarySup.text() != "refURL") {
                QDomAttr primarySupName = primarySup.attributeNode("name");
                qDebug() << primarySupName.value() << primarySupRef.text();
                myTableWidgets[4]->setItem(row, 2, new QTableWidgetItem(primarySupName.value()));
            }
            QDomNodeList secondarySup = doc.elementsByTagName("secondrySupervisor");
            QVector<QString> secondarySupList;
            for (int i = 0; i < secondarySup.size(); i++) {
                QDomNamedNodeMap map = secondarySup.item(i).attributes();
                QDomNode r = secondarySup.item(i);
                QDomElement resourceRef = r.firstChildElement("refURL");
                qDebug() << "Secondary Sup Ref: " << resourceRef.text();
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
                    qDebug() << "Secondary Sup Name: " << attribute.value();
                    secondarySupList.append(attribute.value());
                }
            }
            QListWidget* supervisorListWidget = new QListWidget;
            for (int i = 0; i < secondarySupList.count(); i++) {
                supervisorListWidget->addItem(secondarySupList[i]);
            }
            myTableWidgets[4]->setCellWidget(row, 3, supervisorListWidget);
            myTableWidgets[4]->resizeColumnsToContents();
            QDomNodeList resources = doc.elementsByTagName("resource");
            QVector<QString> resourceList;
            for (int i = 0; i < resources.size(); i++) {
                QDomNamedNodeMap map = resources.item(i).attributes();
                QDomNode r = resources.item(i);
                QDomElement resourceRef = r.firstChildElement("refURL");
                qDebug() << "Resource Ref: " << resourceRef.text();
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
                    qDebug() << "Resource Name: " << attribute.value();
                    resourceList.append(attribute.value());
                }
            }
            QListWidget* resourceListWidget = new QListWidget;
            for (int i = 0; i < resourceList.count(); i++) {
                resourceListWidget->addItem(resourceList[i]);
            }
            myTableWidgets[4]->setCellWidget(row, 4, resourceListWidget);
            myTableWidgets[4]->resizeColumnsToContents();
            QDomNodeList csqs = doc.elementsByTagName("csq");
            QVector<QString> csqList;
            for (int i = 0; i < csqs.size(); i++) {
                QDomNamedNodeMap map = csqs.item(i).attributes();
                QDomNode r = csqs.item(i);
                QDomElement resourceRef = r.firstChildElement("refURL");
                qDebug() << "CSQ Ref: " << resourceRef.text();
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
                    qDebug() << "CSQ Name: " << attribute.value();
                    csqList.append(attribute.value());
                }
            }
            QListWidget* csqListWidget = new QListWidget;
            for (int i = 0; i < csqList.count(); i++) {
                csqListWidget->addItem(csqList[i]);
            }
            myTableWidgets[4]->setCellWidget(row, 5, csqListWidget);
            myTableWidgets[4]->resizeColumnsToContents();
            qDebug() << teamId.text() << teamname.text() << self.text();
        }
    }
}

bool UCCXTabbedWindow::getAllAppData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting app data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/application");
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    } else {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("application");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            appRefLinks.append(self.text());
            qDebug() << appRefLinks[i];
        }
        return true;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedAppData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting app data";
    QUrl req(refURL);
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get team data";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get app data";
    } else {
        qDebug() << "Connected, lets get the app data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("application");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement appName = n.firstChildElement("applicationName");
            QDomElement appId = n.firstChildElement("id");
            QDomElement appType = n.firstChildElement("type");
            QDomElement appDesc = n.firstChildElement("description");
            QDomElement appEnabled = n.firstChildElement("enabled");
            myTableWidgets[2]->insertRow(myTableWidgets[2]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[2]->rowCount();
            int row = myTableWidgets[2]->rowCount() - 1;
            myTableWidgets[2]->setItem(row, 0, new QTableWidgetItem(appName.text()));
            myTableWidgets[2]->setItem(row, 1, new QTableWidgetItem(appId.text()));
            myTableWidgets[2]->setItem(row, 2, new QTableWidgetItem(appType.text()));
            myTableWidgets[2]->setItem(row, 3, new QTableWidgetItem(appDesc.text()));
            myTableWidgets[2]->setItem(row, 4, new QTableWidgetItem(appEnabled.text()));
            myTableWidgets[2]->resizeColumnsToContents();
        }
    }
}

bool UCCXTabbedWindow::getAllSkillData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting skill data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/skill");
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    } else {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("skill");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            skillRefLinks.append(self.text());
            qDebug() << skillRefLinks[i];
        }
        return true;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedSkillData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting skill data";
    QUrl req(refURL);
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get skill data";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get skill data";
    } else {
        qDebug() << "Connected, lets get the skill data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("Skill");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("skillName");
            QDomElement skillId = n.firstChildElement("skillId");
            myTableWidgets[0]->insertRow(myTableWidgets[0]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[0]->rowCount();
            int row = myTableWidgets[0]->rowCount() - 1;
            myTableWidgets[0]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[0]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[0]->resizeColumnsToContents();
        }
    }
}

bool UCCXTabbedWindow::getAllRGData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting RG data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/resourceGroup");
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    } else {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("resourceGroup");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            rgRefLinks.append(self.text());
            qDebug() << rgRefLinks[i];
        }
        return true;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedRGData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting RG data";
    QUrl req(refURL);
    QNetworkRequest request(req);
    request.setRawHeader("Authorization", "Basic " + usernamepassword.toLocal8Bit());
    QNetworkAccessManager test;
    QEventLoop loop;
    connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply * reply = test.get(request);
    reply->ignoreSslErrors(); // Ignore only unsigned later on
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    loop.exec();

    QByteArray response = reply->readAll();
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //progbar.close();//Why does this close, entire application window?
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get RG data";
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get RG data";
    } else {
        qDebug() << "Connected, lets get the RG data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("resourceGroup");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("name");
            QDomElement skillId = n.firstChildElement("id");
            myTableWidgets[1]->insertRow(myTableWidgets[1]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[1]->rowCount();
            int row = myTableWidgets[1]->rowCount() - 1;
            myTableWidgets[1]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[1]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[1]->resizeColumnsToContents();
        }
    }
}

void UCCXTabbedWindow::on_pushButton_clicked()
{
    if (getAllTeamData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        qDebug() << "Got all team ref links";
        for (int i = 0; i < teamRefLinks.count(); i++) {
            teamRefLinks[i].replace("+", "%20");
            getDetailedTeamData(teamRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
    if (getAllAppData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        for (int i = 0; i < appRefLinks.count(); i++) {
            appRefLinks[i].replace("+", "%20");
            getDetailedAppData(appRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
    if (getAllSkillData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        for (int i = 0; i < skillRefLinks.count(); i++) {
            skillRefLinks[i].replace("+", "%20");
            getDetailedSkillData(skillRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
    if (getAllRGData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        for (int i = 0; i < rgRefLinks.count(); i++) {
            rgRefLinks[i].replace("+", "%20");
            getDetailedRGData(rgRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
}