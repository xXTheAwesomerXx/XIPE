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
#include <QDirIterator>
using namespace Variables;
QVector<QTableWidget*> myTableWidgets;
QVector<QString> teamRefLinks, appRefLinks, skillRefLinks, rgRefLinks, csqRefLinks, triggerRefLinks;
QVector<QString> newTeamRefLinks, newAppRefLinks, newSkillRefLinks, newRGRefLinks, newCSQRefLinks, newTriggerRefLinks;
QVector<QString> skillNames, rgNames, appNames, csqNames;
void writeToFile(QString text, QString filePath, QString fileName);
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
        csqHeaders << "Name" << "ID";
        myTableWidgets[3]->setColumnCount(2);
        myTableWidgets[3]->setHorizontalHeaderLabels(csqHeaders);
        triggerHeaders << "Directory Number" << "HRef";
        myTableWidgets[5]->setColumnCount(2);
        myTableWidgets[5]->setHorizontalHeaderLabels(triggerHeaders);
}

UCCXTabbedWindow::~UCCXTabbedWindow()
{
    delete ui;
}

void writeToFile(QString text, QString filePath, QString fileName) {
    QFile logFile(filePath + "/" + fileName);
    QDir logDir;
    if (logDir.exists(filePath)) {
        qDebug() << "path exists";
        if (logFile.open(QIODevice::Append)) {
            qDebug() << "file opened!";
            QTextStream in(&logFile);
                   in << text;
                   logFile.close();
        } else {
            qDebug() << "Failed to open file........";
        }
    } else {
        if (logDir.mkpath(filePath)) {
            qDebug() << "We made the path";
            if (logFile.open(QIODevice::Append)) {
                qDebug() << "file opened!";
                QTextStream in(&logFile);
                       in << text;
                       logFile.close();
            } else {
                qDebug() << "Failed to open file........";
            }
        }
    }
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

    if ( status == 200 || status == 201 || status == 202 ) {
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
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
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

    if ( status == 200 || status == 201 || status == 202 ) {
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
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams", teamname.text() + ".xml");
            qDebug() << teamId.text() << teamname.text() << self.text();
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get team data";
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

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("application");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement appname = n.firstChildElement("applicationName");
            appRefLinks.append(self.text());
            appNames.append(appname.text());
            qDebug() << appRefLinks[i];
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
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

    if ( status == 200 || status == 201 || status == 202 ) {
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
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications", appName.text() + ".xml");
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get app data";
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

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("skill");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement skillName = n.firstChildElement("skillName");
            skillRefLinks.append(self.text());
            skillNames.append(skillName.text());
            qDebug() << skillRefLinks[i];
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
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

    if ( status == 200 || status == 201 || status == 202 ) {
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
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills", skillName.text() + ".xml");
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get skill data";
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

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("resourceGroup");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement name = n.firstChildElement("name");
            rgRefLinks.append(self.text());
            rgNames.append(name.text());
            qDebug() << rgRefLinks[i];
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
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

    if ( status == 200 || status == 201 || status == 202 ) {
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
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups", skillName.text() + ".xml");
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get RG data";
    }
}

bool UCCXTabbedWindow::getAllCSQData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting CSQ data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/csq");
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

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("csq");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement name = n.firstChildElement("name");
            csqRefLinks.append(self.text());
            csqNames.append(name.text());
            qDebug() << "CSQ Name Added: " << name.text();
            qDebug() << csqRefLinks[i];
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedCSQData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting csq data";
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
        qDebug() << "Failed to get csq data";
    }

    int status = statusCode.toInt();

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get the csq data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("csq");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("name");
            qDebug() << skillName.text();
            QDomElement skillId = n.firstChildElement("id");
            myTableWidgets[3]->insertRow(myTableWidgets[3]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[3]->rowCount();
            int row = myTableWidgets[3]->rowCount() - 1;
            myTableWidgets[3]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[3]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[3]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs", skillName.text() + ".xml");
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get csq data";
    }
}

bool UCCXTabbedWindow::getAllTriggerData(QString hostname, QString usernamepassword, QString filePath) {
    qDebug() << "Getting CSQ data";
    QUrl req("https://" + hostname.toLocal8Bit() + "/adminapi/trigger");
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

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get ref links";
        qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("trigger");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomAttr selfURL = self.attributeNode("href");
            triggerRefLinks.append(selfURL.value());
            qDebug() << triggerRefLinks[i];
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get ref. links";
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedTriggerData(QString refURL, QString usernamepassword) {
    qDebug() << "Getting trigger data";
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
        qDebug() << "Failed to get trigger data";
    }

    int status = statusCode.toInt();

    if ( status == 200 || status == 201 || status == 202 ) {
        qDebug() << "Connected, lets get the trigger data";
        //qDebug() << response;
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("trigger");


        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomAttr selfURL = self.attributeNode("href");
            QDomElement dirNum = n.firstChildElement("directoryNumber");
            myTableWidgets[5]->insertRow(myTableWidgets[5]->rowCount());
            qDebug() << "New row count is: " << myTableWidgets[5]->rowCount();
            int row = myTableWidgets[5]->rowCount() - 1;
            myTableWidgets[5]->setItem(row, 0, new QTableWidgetItem(dirNum.text()));
            myTableWidgets[5]->setItem(row, 1, new QTableWidgetItem(selfURL.value()));
            myTableWidgets[5]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers", dirNum.text() + ".xml");
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "Couldn't get trigger data";
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
        qDebug() << rgRefLinks.count() << " RG Count!";
        for (int i = 0; i < rgRefLinks.count(); i++) {
            rgRefLinks[i].replace("+", "%20");
            getDetailedRGData(rgRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
    if (getAllCSQData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        for (int i = 0; i < csqRefLinks.count(); i++) {
            csqRefLinks[i].replace("+", "%20");
            getDetailedCSQData(csqRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
    qDebug() << triggerRefLinks.count() << " Trigger Count!";
    if (getAllTriggerData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
        qDebug() << triggerRefLinks.count() << " Trigger Count!";
        for (int i = 0; i < triggerRefLinks.count(); i++) {
            triggerRefLinks[i].replace("+", "%20");
            getDetailedTriggerData(triggerRefLinks[i], Variables::uccxHostUsernamePwd);
        }
    }
}

void UCCXTabbedWindow::on_pushButton_3_clicked()
{
    //This is Select All, but using it for 'parsing' documents.
    // lets do skills first
    for (int i = 0; i < skillNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills/" + skillNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
                   while (!in.atEnd())
                   {
                        QString line = in.readLine();
                        line = line.replace("+", "%20");// escape the spaces
                        writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills/Parsed", skillNames[i].toLocal8Bit() + ".xml");
                        qDebug() << "Pushing skill data";
                        QUrl req("https://10.0.0.93/adminapi/skill");
                        QNetworkRequest request(req);
                        QByteArray postDataSize = QByteArray::number(line.size());
                        request.setRawHeader("Content-Type", "text/xml");
                        request.setRawHeader("Content-Length", postDataSize);
                        request.setRawHeader("Authorization", "Basic " + Variables::uccxHostUsernamePwd.toLocal8Bit());
                        QNetworkAccessManager test;
                        QEventLoop loop;
                        connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
                        QNetworkReply * reply = test.post(request, line.toLocal8Bit());
                        reply->ignoreSslErrors(); // Ignore only unsigned later on
                        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
                        loop.exec();

                        QByteArray response = reply->readAll();
                        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
                        //progbar.close();//Why does this close, entire application window?
                        if ( !statusCode.isValid() ) {
                            qDebug() << "Failed to get trigger data";
                        }

                        int status = statusCode.toInt();

                        if ( status == 200 || status == 201 || status == 202 ) {
                            qDebug() << "New Skill at: " + response;
                            newSkillRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                        } else {
                            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                        }
                   }
        } else {
            qDebug() << "Couldn't open skill to push!";
        }
    }
    // -- End get files in Dir

    // lets do rgs
    for (int i = 0; i < rgNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups/" + rgNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
                   while (!in.atEnd())
                   {
                        QString line = in.readLine();
                        line = line.replace("+", "%20");// escape the spaces
                        writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups/Parsed", rgNames[i].toLocal8Bit() + ".xml");
                        qDebug() << "Pushing rg data";
                        QUrl req("https://10.0.0.93/adminapi/resourceGroup");
                        QNetworkRequest request(req);
                        QByteArray postDataSize = QByteArray::number(line.size());
                        request.setRawHeader("Content-Type", "text/xml");
                        request.setRawHeader("Content-Length", postDataSize);
                        request.setRawHeader("Authorization", "Basic " + Variables::uccxHostUsernamePwd.toLocal8Bit());
                        QNetworkAccessManager test;
                        QEventLoop loop;
                        connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
                        QNetworkReply * reply = test.post(request, line.toLocal8Bit());
                        reply->ignoreSslErrors(); // Ignore only unsigned later on
                        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
                        loop.exec();

                        QByteArray response = reply->readAll();
                        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
                        //progbar.close();//Why does this close, entire application window?
                        if ( !statusCode.isValid() ) {
                            qDebug() << "Failed to get rg data";
                        }

                        int status = statusCode.toInt();

                        if ( status == 200 || status == 201 || status == 202 ) {
                            qDebug() << "New RG at: " + response;
                            newRGRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                        } else {
                            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                        }
                   }
        } else {
            qDebug() << "Couldn't open rg to push!";
        }
    }
    // -- End get files in Dir

    // lets do apps
    for (int i = 0; i < appNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/" + appNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
                   while (!in.atEnd())
                   {
                        QString line = in.readLine();
                        line = line.replace("+", "%20");// escape the spaces
                        writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/Parsed", rgNames[i].toLocal8Bit() + ".xml");
                        qDebug() << "Pushing app data";
                        QUrl req("https://10.0.0.93/adminapi/application");
                        QNetworkRequest request(req);
                        QByteArray postDataSize = QByteArray::number(line.size());
                        request.setRawHeader("Content-Type", "text/xml");
                        request.setRawHeader("Content-Length", postDataSize);
                        request.setRawHeader("Authorization", "Basic " + Variables::uccxHostUsernamePwd.toLocal8Bit());
                        QNetworkAccessManager test;
                        QEventLoop loop;
                        connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
                        QNetworkReply * reply = test.post(request, line.toLocal8Bit());
                        reply->ignoreSslErrors(); // Ignore only unsigned later on
                        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
                        loop.exec();

                        QByteArray response = reply->readAll();
                        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
                        //progbar.close();//Why does this close, entire application window?
                        if ( !statusCode.isValid() ) {
                            qDebug() << "Failed to get app data";
                        }

                        int status = statusCode.toInt();

                        if ( status == 200 || status == 201 || status == 202 ) {
                            qDebug() << "New app at: " + response;
                            newAppRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                        } else {
                            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                        }
                   }
        } else {
            qDebug() << "Couldn't open app to push!";
        }
    }
    // -- End get files in Dir

    // lets do csqs
    qDebug() << "CSQ Count: " << QString::number(csqNames.count());
    for (int i = 0; i < csqNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/" + csqNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
                        QString line = in.readAll();
                        line = line.replace("+", "%20");// escape the spaces
                        /*Parse CSQ Data*/
                        QDomDocument doc;
                        doc.setContent(line.toLocal8Bit());
                        qDebug() << "Regular:" << line;
                        qDebug() << in.readAll();
                        QDomNodeList csq = doc.elementsByTagName("csq");
                        for (int i = 0; i < csq.size(); i++) {
                            QDomNode n = csq.item(i);
                            QDomElement resourcePoolType = n.firstChildElement("resourcePoolType");
                            QDomElement poolSpecificInfo = n.firstChildElement("poolSpecificInfo");

                            if (resourcePoolType.text() == "SKILL_GROUP") { //if it's not a skill group, why parse for skill groups?
                                QDomElement skillGroup = poolSpecificInfo.firstChildElement("skillGroup");
                                QDomElement skillCompetency = skillGroup.firstChildElement("skillCompetency");
                                QDomElement skillNameUriPair = skillCompetency.firstChildElement("skillNameUriPair");
                                QDomAttr skillNameUriPairName = skillNameUriPair.attributeNode("name"); //csq skill name
                                QDomElement skillRefURL = skillNameUriPair.firstChildElement("refURL");
                                qDebug() << "CSQ Data: " << poolSpecificInfo.text() << skillGroup.text() << skillCompetency.text() << skillNameUriPair.text() << skillNameUriPairName.value() << skillRefURL.text();
                                for (int i = 0; i < skillNames.count(); i++) {
                                    if (skillRefURL.text() == skillRefLinks[i]) {
                                        skillRefURL.firstChild().setNodeValue("A test Ref Link!");
                                    }
                                }
                            } else if (resourcePoolType.text() == "RESOURCE_GROUP") { //if it's not a skill group, why parse for skill groups?
                                QDomElement resourceGroup = poolSpecificInfo.firstChildElement("resourceGroup");
                                QDomElement resourceGroupNameUriPair = resourceGroup.firstChildElement("resourceGroupNameUriPair");
                                QDomAttr resourceGroupUriPairName = resourceGroupNameUriPair.attributeNode("name"); //csq rg name
                                QDomElement resourceRefURL = resourceGroupNameUriPair.firstChildElement("refURL");
                                for (int i = 0; i < rgNames.count(); i++) {
                                    if (resourceRefURL.text() == rgRefLinks[i]) {
                                        resourceRefURL.firstChild().setNodeValue("A test rg Ref Link!");
                                    }
                                }
                            }
                        }
                        writeToFile(doc.toString(), QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/Parsed", csqNames[i].toLocal8Bit() + ".xml");
                        qDebug() << "Pushing csq data";
                   } else {
            qDebug() << "Couldn't open app to push!";
        }
    }
    // -- End get files in Dir
}
