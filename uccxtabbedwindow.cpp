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
QVector<QString> skillNames, rgNames, appNames, csqNames, teamNames, triggerNames;
void writeToFile(QString text, QString filePath, QString fileName);
UCCXTabbedWindow::UCCXTabbedWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UCCXTabbedWindow)
{
    ui->setupUi(this);
    //Variables::uccxClientIP = Variables::uccxHostIP;
    //Variables::uccxClientUsernamePwd = Variables::uccxHostUsernamePwd;
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

void UCCXTabbedWindow::writeToLogAndStatus(QString text) {
    ui->statusbar->showMessage(text);
    appendToFile(text, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
}

void UCCXTabbedWindow::appendToFile(QString text, QString filePath, QString fileName) {
    QFile logFile(filePath + "/" + fileName);
    QDir logDir;
    if (logDir.exists(filePath)) {
        if (logFile.open(QIODevice::Append)) {
            QTextStream in(&logFile);
                   in << text << endl;
                   in << "-------------------------------------------------" << endl;
                   logFile.close();
        }
    } else {
        if (logDir.mkpath(filePath)) {
            if (logFile.open(QIODevice::Append)) {
                QTextStream in(&logFile);
                    in << text << endl;
                    in << "-------------------------------------------------" << endl;
                       logFile.close();
            }
        }
    }
}

void writeToFile(QString text, QString filePath, QString fileName) {
    QFile logFile(filePath + "/" + fileName);
    QDir logDir;
    if (logDir.exists(filePath)) {
        if (logFile.open(QIODevice::Append)) {
            QTextStream in(&logFile);
            in << text;
            logFile.close();
        }
    } else {
        if (logDir.mkpath(filePath)) {
            if (logFile.open(QIODevice::Append)) {
                QTextStream in(&logFile);
                in << text;
                logFile.close();
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
    writeToLogAndStatus("Getting Team Reference links!");
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
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status == 200 || status == 201 || status == 202 ) {
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList teams = doc.elementsByTagName("team");
        for (int i = 0; i < teams.size(); i++) {
            QDomNode n = teams.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement teamId = n.firstChildElement("teamId");
            QDomElement teamname = n.firstChildElement("teamname");
            if (teamname.text() != "Default") { //Lets not get the Default team.
                teamRefLinks.append(self.text());
                teamNames.append(teamname.text());
                writeToLogAndStatus("Found Team " + teamname.text() + " at " + self.text());
            } else {
                writeToLogAndStatus("Found 'Default' team, skipping!");
            }
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Team Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedTeamData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed Team data for " + refURL);
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
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get team data";
    }

    int status = statusCode.toInt();

    if ( status == 200 || status == 201 || status == 202 ) {
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
            int row = myTableWidgets[4]->rowCount() - 1;
            myTableWidgets[4]->setItem(row, 0, new QTableWidgetItem(teamname.text()));
            myTableWidgets[4]->setItem(row, 1, new QTableWidgetItem(teamId.text()));
            if (primarySup.text() != "refURL") {
                QDomAttr primarySupName = primarySup.attributeNode("name");
                myTableWidgets[4]->setItem(row, 2, new QTableWidgetItem(primarySupName.value()));
            }
            QDomNodeList secondarySup = doc.elementsByTagName("secondrySupervisor");
            QVector<QString> secondarySupList;
            for (int i = 0; i < secondarySup.size(); i++) {
                QDomNamedNodeMap map = secondarySup.item(i).attributes();
                QDomNode r = secondarySup.item(i);
                QDomElement resourceRef = r.firstChildElement("refURL");
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
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
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
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
                for (int j = 0; j < map.length(); j++)  {
                    QDomNode mapItem = map.item(j);
                    QDomAttr attribute = mapItem.toAttr();
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
            writeToLogAndStatus("Successfully obtained detailed Team data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Team data from " + refURL);
    }
}

bool UCCXTabbedWindow::getAllAppData(QString hostname, QString usernamepassword, QString filePath) {
    writeToLogAndStatus("Getting Application Reference links!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("application");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement appname = n.firstChildElement("applicationName");
            appRefLinks.append(self.text());
            appNames.append(appname.text());
            writeToLogAndStatus("Found Application " + appname.text() + " at " + self.text());
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Application Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedAppData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed Application data!");
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
            int row = myTableWidgets[2]->rowCount() - 1;
            myTableWidgets[2]->setItem(row, 0, new QTableWidgetItem(appName.text()));
            myTableWidgets[2]->setItem(row, 1, new QTableWidgetItem(appId.text()));
            myTableWidgets[2]->setItem(row, 2, new QTableWidgetItem(appType.text()));
            myTableWidgets[2]->setItem(row, 3, new QTableWidgetItem(appDesc.text()));
            myTableWidgets[2]->setItem(row, 4, new QTableWidgetItem(appEnabled.text()));
            myTableWidgets[2]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications", appName.text() + ".xml");
            writeToLogAndStatus("Successfully obtained Application data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Application data from " + refURL);
    }
}

bool UCCXTabbedWindow::getAllSkillData(QString hostname, QString usernamepassword, QString filePath) {
    writeToLogAndStatus("Getting Skill Reference links!");
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
    if ( !statusCode.isValid() ) {
        qDebug() << "Failed to get ref links";
    }

    int status = statusCode.toInt();

    if ( status == 200 || status == 201 || status == 202 ) {
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("skill");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement skillName = n.firstChildElement("skillName");
            skillRefLinks.append(self.text());
            skillNames.append(skillName.text());
            writeToLogAndStatus("Found Skill " + skillName.text() + " at " + self.text());
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Skill Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedSkillData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed Skill data!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("Skill");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("skillName");
            QDomElement skillId = n.firstChildElement("skillId");
            myTableWidgets[0]->insertRow(myTableWidgets[0]->rowCount());
            int row = myTableWidgets[0]->rowCount() - 1;
            myTableWidgets[0]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[0]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[0]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills", skillName.text() + ".xml");
            writeToLogAndStatus("Successfully obtained Skill data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Skill data from " + refURL);
    }
}

bool UCCXTabbedWindow::getAllRGData(QString hostname, QString usernamepassword, QString filePath) {
    writeToLogAndStatus("Getting Resource Group Reference links");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("resourceGroup");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement name = n.firstChildElement("name");
            rgRefLinks.append(self.text());
            rgNames.append(name.text());
            writeToLogAndStatus("Found Resource Group " + name.text() + " at " + self.text());
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Resource Group Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedRGData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed Resource Group data!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("resourceGroup");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("name");
            QDomElement skillId = n.firstChildElement("id");
            myTableWidgets[1]->insertRow(myTableWidgets[1]->rowCount());
            int row = myTableWidgets[1]->rowCount() - 1;
            myTableWidgets[1]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[1]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[1]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups", skillName.text() + ".xml");
            writeToLogAndStatus("Successfully obtained Resource Group data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Resource Group data from " + refURL);
    }
}

bool UCCXTabbedWindow::getAllCSQData(QString hostname, QString usernamepassword, QString filePath) {
    writeToLogAndStatus("Getting CSQ Reference links!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("csq");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement name = n.firstChildElement("name");
            csqRefLinks.append(self.text());
            csqNames.append(name.text());
            writeToLogAndStatus("Found CSQ " + name.text() + " at " + self.text());
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain CSQ Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedCSQData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed CSQ data!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("csq");
        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement skillName = n.firstChildElement("name");
            QDomElement skillId = n.firstChildElement("id");
            myTableWidgets[3]->insertRow(myTableWidgets[3]->rowCount());
            int row = myTableWidgets[3]->rowCount() - 1;
            myTableWidgets[3]->setItem(row, 0, new QTableWidgetItem(skillName.text()));
            myTableWidgets[3]->setItem(row, 1, new QTableWidgetItem(skillId.text()));
            myTableWidgets[3]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs", skillName.text() + ".xml");
            writeToLogAndStatus("Successfully obtained CSQ data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain CSQ data from " + refURL);
    }
}

bool UCCXTabbedWindow::getAllTriggerData(QString hostname, QString usernamepassword, QString filePath) {
    writeToLogAndStatus("Getting Trigger Reference links!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList apps = doc.elementsByTagName("trigger");
        for (int i = 0; i < apps.size(); i++) {
            QDomNode n = apps.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomElement dirNum = n.firstChildElement("directoryNumber");
            QDomAttr selfURL = self.attributeNode("href");
            triggerRefLinks.append(selfURL.value());
            triggerNames.append(dirNum.text());
            writeToLogAndStatus("Found Trigger " + dirNum.text() + " at " + selfURL.value());
        }
        return true;
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Trigger Reference links!");
        return false;
    }
    return false;
}

void UCCXTabbedWindow::getDetailedTriggerData(QString refURL, QString usernamepassword) {
    writeToLogAndStatus("Getting detailed Trigger data!");
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
        QDomDocument doc;
        doc.setContent(response);
        QDomNodeList app = doc.elementsByTagName("trigger");


        for (int i = 0; i < app.size(); i++) {
            QDomNode n = app.item(i);
            QDomElement self = n.firstChildElement("self");
            QDomAttr selfURL = self.attributeNode("href");
            QDomElement dirNum = n.firstChildElement("directoryNumber");
            myTableWidgets[5]->insertRow(myTableWidgets[5]->rowCount());
            int row = myTableWidgets[5]->rowCount() - 1;
            myTableWidgets[5]->setItem(row, 0, new QTableWidgetItem(dirNum.text()));
            myTableWidgets[5]->setItem(row, 1, new QTableWidgetItem(selfURL.value()));
            myTableWidgets[5]->resizeColumnsToContents();
            writeToFile(response, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers", dirNum.text() + ".xml");
            writeToLogAndStatus("Successfully obtain Trigger data from " + refURL);
        }
    } else {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        writeToLogAndStatus("Failed to obtain Trigger data from " + refURL);
    }
}

void UCCXTabbedWindow::on_btnPushData_clicked()
{
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
                QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/skill");
                QNetworkRequest request(req);
                QByteArray postDataSize = QByteArray::number(line.size());
                request.setRawHeader("Content-Type", "text/xml");
                request.setRawHeader("Content-Length", postDataSize);
                request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                    QString newStringToAdd = QString::fromLocal8Bit(response);
                    newSkillRefLinks.append(newStringToAdd);//this will cause the new ref link to be in the same index
                    writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills/Parsed/Finished", skillNames[i].toLocal8Bit() + ".xml");
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
                QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/resourceGroup");
                QNetworkRequest request(req);
                QByteArray postDataSize = QByteArray::number(line.size());
                request.setRawHeader("Content-Type", "text/xml");
                request.setRawHeader("Content-Length", postDataSize);
                request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                    writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups/Parsed/Finished", rgNames[i].toLocal8Bit() + ".xml");
                } else {
                    QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
                }
            }
        } else {
            qDebug() << "Couldn't open rg to push!";
        }
    }
    // -- End get files in Dir
    // lets parse apps
    qDebug() << "App Count: " << QString::number(appNames.count());
    for (int i = 0; i < appNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/" + appNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            line = line.replace("///", "//");//Why are there three backslashes to a url?
            /*Parse CSQ Data*/
            QDomDocument doc;
            doc.setContent(line.toLocal8Bit());
            qDebug() << in.readAll();
            QDomNodeList csq = doc.elementsByTagName("application");
            for (int i = 0; i < csq.size(); i++) {
                QDomElement applicationName = csq.at(i).firstChildElement("applicationName");
                applicationName.firstChild().setNodeValue(applicationName.text().replace(" ", ""));
                //Replace the space in the application name,
                //because if we don't we get an error!
            }
            QString documentString = doc.toString();
            documentString = documentString.replace(Variables::uccxHostIP, Variables::uccxClientIP);
            writeToFile(documentString, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/Parsed", appNames[i].toLocal8Bit() + ".xml");
            qDebug() << "Pushing app data";

        } else {
            qDebug() << "Couldn't open app to push!";
        }
    } //At this point, we now know all Apps have been parsed
    // -- End parse files in Dir
    // lets do apps
    for (int i = 0; i < appNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/Parsed/" + appNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            qDebug() << "Pushing app data";
            QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/application");
            QNetworkRequest request(req);
            QByteArray postDataSize = QByteArray::number(line.size());
            request.setRawHeader("Content-Type", "text/xml");
            request.setRawHeader("Content-Length", postDataSize);
            request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                qDebug() << "Failed to get csq data";
            }

            int status = statusCode.toInt();

            if ( status == 200 || status == 201 || status == 202 ) {
                qDebug() << "New app at: " + response;
                newAppRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications/Parsed/Finished", appNames[i].toLocal8Bit() + ".xml");
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            }
        } else {
            qDebug() << "Couldn't open app to push!";
        }
    }
    // -- End get files in Dir
    // lets parse csqs
    qDebug() << "CSQ Count: " << QString::number(csqNames.count());
    for (int i = 0; i < csqNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/" + csqNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            line = line.replace("///", "//");//Why are there three backslashes to a url?
            /*Parse CSQ Data*/
            QDomDocument doc;
            doc.setContent(line.toLocal8Bit());
            qDebug() << in.readAll();
            QDomNodeList csq = doc.elementsByTagName("csq");
            for (int i = 0; i < csq.size(); i++) {

                QDomNode n = csq.item(i);
                QDomElement resourcePoolType = n.firstChildElement("resourcePoolType");
                QDomElement poolSpecificInfo = n.firstChildElement("poolSpecificInfo");

                if (resourcePoolType.text() == "SKILL_GROUP") { //if it's not a skill group, why parse for skill groups?
                    QDomElement skillGroup = poolSpecificInfo.firstChildElement("skillGroup");
                    QDomNodeList skillCompetency = skillGroup.elementsByTagName("skillCompetency");
                    for (int e = 0; e < skillCompetency.size(); e++) {
                        QDomNode skillSet = skillCompetency.item(e);
                        QDomElement skillNameUriPair = skillSet.firstChildElement("skillNameUriPair");
                        QDomAttr skillNameUriPairName = skillNameUriPair.attributeNode("name"); //csq skill name
                        QDomElement skillRefURL = skillNameUriPair.firstChildElement("refURL");
                        for (int i = 0; i < skillNames.count(); i++) {
                            qDebug() << "Old Ref: " + skillRefLinks[i].toLocal8Bit() + " New Ref Link:" + newSkillRefLinks[i].toLocal8Bit();
                            if (skillRefURL.text() == skillRefLinks[i]) {
                                qDebug() << "Replaced: " + skillRefURL.text() + " with, " + newSkillRefLinks[i].toLocal8Bit();
                                skillRefURL.firstChild().setNodeValue(newSkillRefLinks[i]);
                            }
                        }
                    }
                } else if (resourcePoolType.text() == "RESOURCE_GROUP") { //if it's not a skill group, why parse for skill groups?
                    QDomElement resourceGroup = poolSpecificInfo.firstChildElement("resourceGroup");
                    QDomElement resourceGroupNameUriPair = resourceGroup.firstChildElement("resourceGroupNameUriPair");
                    QDomAttr resourceGroupUriPairName = resourceGroupNameUriPair.attributeNode("name"); //csq rg name
                    QDomElement resourceRefURL = resourceGroupNameUriPair.firstChildElement("refURL");
                    for (int i = 0; i < rgNames.count(); i++) {
                        if (resourceRefURL.text() == rgRefLinks[i]) {
                            resourceRefURL.firstChild().setNodeValue(newRGRefLinks[i]);
                        }
                    }
                }

            }
            QString documentString = doc.toString();
            documentString = documentString.replace(Variables::uccxHostIP, Variables::uccxClientIP);
            writeToFile(documentString, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/Parsed", csqNames[i].toLocal8Bit() + ".xml");
            qDebug() << "Pushing csq data";

        } else {
            qDebug() << "Couldn't open csq to push!";
        }
    } //At this point, we now know all CSQs have been parsed
    // -- End parse files in Dir

    // lets do csqs
    for (int i = 0; i < csqNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/Parsed/" + csqNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            qDebug() << "Pushing csq data";
            QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/csq");
            QNetworkRequest request(req);
            QByteArray postDataSize = QByteArray::number(line.size());
            request.setRawHeader("Content-Type", "text/xml");
            request.setRawHeader("Content-Length", postDataSize);
            request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                qDebug() << "Failed to get csq data";
            }

            int status = statusCode.toInt();

            if ( status == 200 || status == 201 || status == 202 ) {
                qDebug() << "New csq at: " + response;
                newCSQRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs/Parsed/Finished", csqNames[i].toLocal8Bit() + ".xml");
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            }
        } else {
            qDebug() << "Couldn't open csq to push!";
        }
    }
    // -- End get files in Dir

    // lets parse teams
    qDebug() << "Team Count: " << QString::number(teamNames.count());
    for (int i = 0; i < teamNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams/" + teamNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            line = line.replace("///", "//");//Why are there three backslashes to a url?
            /*Parse CSQ Data*/
            QDomDocument doc;
            doc.setContent(line.toLocal8Bit());
            qDebug() << in.readAll();
            QDomNodeList csqs = doc.elementsByTagName("csq");
            for (int c = 0; c < csqs.size(); c++) {
                QDomNode r = csqs.item(c);
                QDomElement resourceRef = r.firstChildElement("refURL");
                qDebug() << "Team CSQ Ref: " << resourceRef.text();
                for (int i = 0; i < csqNames.count(); i++) {
                    if (resourceRef.text() == csqRefLinks[i]) {
                        qDebug() << "Replaced a CSQ: " + resourceRef.text() + " with " + newCSQRefLinks[i].toLocal8Bit();
                        resourceRef.firstChild().setNodeValue(newCSQRefLinks[i]);
                    }
                }
            }
            QString documentString = doc.toString();
            documentString = documentString.replace(Variables::uccxHostIP, Variables::uccxClientIP);
            writeToFile(documentString, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams/Parsed", teamNames[i].toLocal8Bit() + ".xml");
            qDebug() << "Pushing team data";

        } else {
            qDebug() << "Couldn't open team to push!";
        }
    } //At this point, we now know all teams have been parsed
    // -- End parse files in Dir
    // lets do teams
    for (int i = 0; i < teamNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams/Parsed/" + teamNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            qDebug() << "Pushing team data";
            QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/team");
            QNetworkRequest request(req);
            QByteArray postDataSize = QByteArray::number(line.size());
            request.setRawHeader("Content-Type", "text/xml");
            request.setRawHeader("Content-Length", postDataSize);
            request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                qDebug() << "Failed to get team data";
            }

            int status = statusCode.toInt();

            if ( status == 200 || status == 201 || status == 202 ) {
                qDebug() << "New team at: " + response;
                newTeamRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams/Parsed/Finished", teamNames[i].toLocal8Bit() + ".xml");
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            }
        } else {
            qDebug() << "Couldn't open team to push!";
        }
    }
    // -- End get files in Dir

    // lets parse triggers
    qDebug() << "Trigger Count: " << QString::number(triggerNames.count());
    for (int i = 0; i < triggerNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers/" + triggerNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            line = line.replace("///", "//");//Why are there three backslashes to a url?
            /*Parse trigger Data*/
            QDomDocument doc;
            doc.setContent(line.toLocal8Bit());
            qDebug() << in.readAll();
            QDomNodeList csqs = doc.elementsByTagName("trigger");
            for (int c = 0; c < csqs.size(); c++) {
                QDomNode r = csqs.item(c);
                QDomElement applicationReplace = r.firstChildElement("application");
                QDomNodeList selfReplace = doc.elementsByTagName("self");
                for (int x = 0; x < selfReplace.count(); x++) {
                    qDebug() << "Replaced an application, in a team!";
                    doc.removeChild(selfReplace.at(x));//should replace self urls
                }
                QDomElement resourceRef = applicationReplace.firstChildElement("refURL");
                qDebug() << "Team App Ref: " << resourceRef.text();
                for (int i = 0; i < appNames.count(); i++) {
                    qDebug() << "App Name: " + appNames[i] + " Old Ref: " + appRefLinks[i] + " New: " + newAppRefLinks[i];
                    if (resourceRef.text().replace("+", "%20") == appRefLinks[i]) {
                        qDebug() << "Replaced an App: " + resourceRef.text().replace("+", "%20") + " with " + newAppRefLinks[i].toLocal8Bit();
                        resourceRef.firstChild().setNodeValue(newAppRefLinks[i]);
                    }
                }
                qDebug() << "New Apps Size: " << newAppRefLinks.count();
                for (int i = 0; i < newAppRefLinks.count(); i++) {
                    qDebug() << "New App Ref: " << newAppRefLinks[i].toLocal8Bit();
                }
            }
            QString documentString = doc.toString();
            documentString = documentString.replace(Variables::uccxHostIP, Variables::uccxClientIP);
            writeToFile(documentString, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers/Parsed", triggerNames[i].toLocal8Bit() + ".xml");
            qDebug() << "Pushing trigger data";

        } else {
            qDebug() << "Couldn't open trigger to push!";
        }
    } //At this point, we now know all teams have been parsed
    // -- End parse files in Dir

    // lets do triggers
    for (int i = 0; i < triggerNames.count(); i++) {
        QFile file(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers/Parsed/" + triggerNames[i].toLocal8Bit() + ".xml");
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QString line = in.readAll();
            line = line.replace("+", "%20");// escape the spaces
            qDebug() << "Pushing trigger data";
            QUrl req("https://" + Variables::uccxClientIP.toLocal8Bit() + "/adminapi/trigger");
            QNetworkRequest request(req);
            QByteArray postDataSize = QByteArray::number(line.size());
            request.setRawHeader("Content-Type", "text/xml");
            request.setRawHeader("Content-Length", postDataSize);
            request.setRawHeader("Authorization", "Basic " + Variables::uccxClientUsernamePwd.toLocal8Bit());
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
                qDebug() << "Failed to get team data";
            }

            int status = statusCode.toInt();

            if ( status == 200 || status == 201 || status == 202 ) {
                qDebug() << "New trigger at: " + response;
                newTriggerRefLinks.append(QString(response));//this will cause the new ref link to be in the same index
                writeToFile(line, QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers/Parsed/Finished", triggerNames[i].toLocal8Bit() + ".xml");
            } else {
                QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            }
        } else {
            qDebug() << "Couldn't open trigger to push!";
        }
    }
    // -- End get files in Dir
}

void UCCXTabbedWindow::on_btnGetData_clicked()
{
    if (!(ui->checkboxApps->checkState() == Qt::Checked && ui->checkboxCSQs->checkState() == Qt::Checked && ui->checkboxRGs->checkState() == Qt::Checked && ui->checkboxSkills->checkState() == Qt::Checked && ui->checkboxTeams->checkState() == Qt::Checked && ui->checkboxTriggers->checkState() == Qt::Checked)) {
        //error
        ui->statusbar->showMessage("No data types were selected! Please select at least one data type to continue.");
    } else {
        ui->btnGetData->setEnabled(false);
        ui->checkboxApps->setEnabled(false);
        ui->checkboxCSQs->setEnabled(false);
        ui->checkboxRGs->setEnabled(false);
        ui->checkboxSkills->setEnabled(false);
        ui->checkboxTeams->setEnabled(false);
        ui->checkboxTriggers->setEnabled(false);
        ui->btnSelectAll->setEnabled(false);
        ui->btnUncheckAll->setEnabled(false);
        if (ui->checkboxTeams->checkState() == Qt::Checked) {
            if (getAllTeamData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all team data!");
                for (int i = 0; i < teamRefLinks.count(); i++) {
                    teamRefLinks[i].replace("+", "%20");
                    getDetailedTeamData(teamRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        if (ui->checkboxApps->checkState() == Qt::Checked) {
            if (getAllAppData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all application data!");
                for (int i = 0; i < appRefLinks.count(); i++) {
                    appRefLinks[i].replace("+", "%20");//dont replace these, because the full uri is used for parsing
                    getDetailedAppData(appRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        if (ui->checkboxSkills->checkState() == Qt::Checked) {
            if (getAllSkillData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all skill data!");
                for (int i = 0; i < skillRefLinks.count(); i++) {
                    skillRefLinks[i].replace("+", "%20");
                    getDetailedSkillData(skillRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        if (ui->checkboxRGs->checkState() == Qt::Checked) {
            if (getAllRGData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all resource group data!");
                for (int i = 0; i < rgRefLinks.count(); i++) {
                    rgRefLinks[i].replace("+", "%20");
                    getDetailedRGData(rgRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        if (ui->checkboxCSQs->checkState() == Qt::Checked) {
            if (getAllCSQData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all csq data!");
                for (int i = 0; i < csqRefLinks.count(); i++) {
                    csqRefLinks[i].replace("+", "%20");
                    getDetailedCSQData(csqRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        if (ui->checkboxTriggers->checkState() == Qt::Checked) {
            if (getAllTriggerData(Variables::uccxHostIP, Variables::uccxHostUsernamePwd, "")) {
                ui->statusbar->showMessage("Finished obtaining all trigger data!");
                for (int i = 0; i < triggerRefLinks.count(); i++) {
                    triggerRefLinks[i].replace("+", "%20");
                    getDetailedTriggerData(triggerRefLinks[i], Variables::uccxHostUsernamePwd);
                }
            }
        }
        ui->btnPushData->setEnabled(true);
        ui->statusbar->showMessage("Finished obtaining all selected data types! Ready to begin migration!");
    }
}

void UCCXTabbedWindow::on_btnSelectAll_clicked()
{
    ui->checkboxApps->setChecked(true);
    ui->checkboxCSQs->setChecked(true);
    ui->checkboxRGs->setChecked(true);
    ui->checkboxSkills->setChecked(true);
    ui->checkboxTeams->setChecked(true);
    ui->checkboxTriggers->setChecked(true);
}

void UCCXTabbedWindow::on_btnUncheckAll_clicked()
{
    ui->checkboxApps->setChecked(false);
    ui->checkboxCSQs->setChecked(false);
    ui->checkboxRGs->setChecked(false);
    ui->checkboxSkills->setChecked(false);
    ui->checkboxTeams->setChecked(false);
    ui->checkboxTriggers->setChecked(false);
}
