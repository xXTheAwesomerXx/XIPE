#include "endusersclustermngmttabswindow.h"
#include "ui_endusersclustermngmttabswindow.h"
#include "clustertab.h"
#include "variables.h"
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
#include <QMessageBox>
#include <QDomDocument>
#include <QColor>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDir>
#include "QScrollBar"

using namespace Variables;
QVector<QListWidget*> myList;
QVector<QListWidget*> myList2;
QVector<QString*> messageString;
EndusersClusterMngmtTabsWindow::EndusersClusterMngmtTabsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EndusersClusterMngmtTabsWindow)
{
    ui->setupUi(this);
    myList.clear();
    myList2.clear();
    messageString.clear();
    if (Variables::logInterfaceShowing == true) {
        ui->actionLog_Interface->setChecked(true);
    }
    for (int i = 0; i < Variables::clusterNamesF.count(); i++) {
        ClusterTab * tab = new ClusterTab();
        tab->listWidget->setStyleSheet("QListView::item:selected:active { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(58, 58, 58, 255), stop:1 rgba(90, 90, 90, 255)) } QListView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1); }");
        tab->listWidget2->setStyleSheet("QListView::item:selected:active { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(58, 58, 58, 255), stop:1 rgba(90, 90, 90, 255)) } QListView::item:hover { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1); }");
        tab->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
        tab->listWidget2->setSelectionMode(QAbstractItemView::MultiSelection);
        myList.append(tab->listWidget2);
        myList2.append(tab->listWidget);
        QString* string = new QString("");
        messageString.append(string);
        ui->tabWidgetClusters->addTab(tab, Variables::clusterNamesF[i]);
    }
    QStringList queryTypeList, queryParameter1;
    queryParameter1 << "Equals" << "Contains" << "Begins with" << "Ends with" << "Is Empty" << "Is not Empty";
    queryTypeList << "Firstname" << "Middlename" << "Lastname" << "UserID" << "Is in Cluster" << "Telephone Number";
    ui->comboBoxType->addItems(queryTypeList);
    ui->comboBoxParameter->addItems(queryParameter1);
}

EndusersClusterMngmtTabsWindow::~EndusersClusterMngmtTabsWindow()
{
    delete ui;
}

void EndusersClusterMngmtTabsWindow::appendToFile(QString text, QString filePath, QString fileName) {
    QFile logFile(filePath + "/" + fileName);
    QDir logDir;
    if (logDir.exists(filePath)) {
        if (logFile.open(QIODevice::Append)) {
            QTextStream in(&logFile);
                   in << "[" << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString().replace(":", "-") << "] " << text << endl;
                   in << "-------------------------------------------------" << endl;
                   logFile.close();
        }
        if (logFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&logFile);
            Variables::logInterface->setLogText(in.readAll());
            Variables::logScrollBar->setValue(2147483647);
        }
    } else {
        if (logDir.mkpath(filePath)) {
            if (logFile.open(QIODevice::Append)) {
                QTextStream in(&logFile);
                    in << "[" << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString().replace(":", "-") << "] " << text << endl;
                    in << "-------------------------------------------------" << endl;
                       logFile.close();
            }
        }
        if (logFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&logFile);
            Variables::logInterface->setLogText(in.readAll());
            Variables::logScrollBar->setValue(2147483647);
        }
    }
}

QString EndusersClusterMngmtTabsWindow::getSubstringBetween(QString src, QString start, QString stop) {
    std::string sourceStd = src.toStdString();
    std::string startStd = start.toStdString();
    std::string stopStd = stop.toStdString();
    unsigned first = sourceStd.find(startStd);
    unsigned last = sourceStd.find(stopStd);
    return QString::fromStdString(sourceStd.substr(first, last - first).erase(0, start.length()));
}

void EndusersClusterMngmtTabsWindow::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void EndusersClusterMngmtTabsWindow::onError(QNetworkReply::NetworkError rep) {
    appendToFile("Encountered an error while attempting to establish a secure connection! Retrying...", QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
}

void EndusersClusterMngmtTabsWindow::findUsers(QString hostname, QString usernamepassword, QString version, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2, int itemIndex) {
    //First maybe we clear the dang list?!
    QString condition1String, condition2String;
    if (condition1 == 0) {
        condition1String = "enduser.firstname";
    } else if (condition1 == 1) {
        condition1String = "enduser.middlename";
    } else if (condition1 == 2) {
        condition1String = "enduser.lastname";
    } else if (condition1 == 3) {
        condition1String = "enduser.userid";
    } else if (condition1 == 4) {
        condition1String = "enduser.islocaluser";
    } else if (condition1 == 5) {
        condition1String = "enduser.telephonenumber";
    }

    if (condition2 == "Equals") {
        condition2String = "= '" + argument.toLower() + "'";
    } else if (condition2 == "Contains") {
        condition2String = "LIKE '%" + argument.toLower() + "%'";
    } else if (condition2 == "Begins with") {
        condition2String = "LIKE '" + argument.toLower() + "%'";
    } else if (condition2 == "Ends with") {
        condition2String = "LIKE '%" + argument.toLower() + "'";
    } else if (condition2 == "Is Empty") {
        condition2String = "IS NULL";
    } else if (condition2 == "Is not Empty") {
        condition2String = "IS NOT NULL";
    } else if (condition2 == "True") {
        condition2String = "= 't'";
    } else if (condition2 == "False") {
        condition2String = "= 'f'";
    }

    QByteArray jsonString;
    if (version != "8.x") {
    jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
    if (condition1 == 4) {
        jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where "
                + condition1String + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    } else {
    jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where lower("
            + condition1String + ") " + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    }
    } else {
        jsonString = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
        if (condition1 == 4) {
            jsonString += "<SOAP-ENV:Body><axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\"><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where "
                    + condition1String + condition2String + "</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>";
        } else {
        jsonString += "<SOAP-ENV:Body><axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\"><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where lower("
                + condition1String + ") " + condition2String + "</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>";
        }
    }
    QString jsonLogString;
    if (version != "8.x") {
        jsonLogString = getSubstringBetween(jsonString, "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\"><soapenv:Body><ns:executeSQLQuery><sql>", "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>");
    } else {
        jsonLogString = getSubstringBetween(jsonString, "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"><SOAP-ENV:Body><axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\"><sql>", "</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>");
    }
    appendToFile("Execute Query: " + jsonLogString.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
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

    if ( !statusCode.isValid() ) {
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        appendToFile("Failed to connect to " + hostname.toLocal8Bit() + " for find users query!", QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
    } else {
        QDomDocument doc;
        doc.setContent(response);
            QDomNodeList rates = doc.elementsByTagName("row");
            for (int i = 0; i < rates.size(); i++) {
                QString finalString;
                QDomNode n = rates.item(i);
                QDomElement firstname = n.firstChildElement("firstname");
                if (firstname.text() != "")
                    finalString = firstname.text() + " ";
                QDomElement middlename = n.firstChildElement("middlename");
                if (middlename.text() != "")
                    finalString += middlename.text() + " ";
                QDomElement lastname = n.firstChildElement("lastname");
                if (lastname.text() != " ")
                    finalString += lastname.text();
                QDomElement userid = n.firstChildElement("userid");
                finalString += " (" + userid.text() + ") ";
                QDomElement islocaluser = n.firstChildElement("islocaluser");
                QDomElement telephonenumber = n.firstChildElement("telephonenumber");
                    if (telephonenumber.text() != "") {
                        finalString += " [" + telephonenumber.text() + "]";
                    } else {
                        finalString += " [No Telephone Number]";
                    }
                QListWidgetItem* user = new QListWidgetItem(finalString);
                if (islocaluser.text() == "t") {
                    user->setBackground(QColor(0, 170, 255));
                    list2->addItem(user);
                } else {
                    user->setBackground(QColor(170, 0, 0));
                    list->addItem(user);
                }
            }
            QString* messageStringToAdd = new QString("Query returned " + QString::number(rates.size()) + " results, " + QString::number(list2->count()) + " Endusers reside in this cluster and " + QString::number(list->count()) + " Endusers do not!");
            messageString[itemIndex] = messageStringToAdd;
            QString logString(messageStringToAdd->toLocal8Bit());
            appendToFile(logString, QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
    }
}

void EndusersClusterMngmtTabsWindow::addUsersToCluster(QString hostname, QString usernamepassword, QString version, QListWidget *list1, QListWidget *list2, QStatusBar * statusbar) {
    int goodAdditions = 0, badAdditions = 0, totalUpdates = list1->selectedItems().count();
    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString;
        if (version != "8.x") {
            jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
            jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 't' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        } else {
            jsonString = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
            jsonString += "<SOAP-ENV:Body><axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\">";
            jsonString += "<sql>UPDATE enduser SET enduser.islocaluser = 't' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>";
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

        if ( !statusCode.isValid() ) {
            badAdditions++;
        }

        int status = statusCode.toInt();

        if ( status != 200 ) {
            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
            appendToFile("Failed to connect to " + hostname.toLocal8Bit() + " for add user update on userid: " + userid.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
        } else {
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(0, 170, 255));//Correct the color scheme
                goodAdditions++;
                appendToFile("Successfully added userid " + userid.toLocal8Bit() + " to cluster " + hostname.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
            } else {
                badAdditions++;
                appendToFile("Failed to add user id " + userid.toLocal8Bit() + " to cluster " + hostname.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
            }
        }
        statusbar->showMessage("Successful Updates: " + QString::number(goodAdditions) + " | Failed Updates: " + QString::number(badAdditions) + " | Progress: " + QString::number(badAdditions + goodAdditions) + "/" + QString::number(totalUpdates) + " | " + QString::number(totalUpdates - (goodAdditions + badAdditions)) + " left");
        }
    }
}

void EndusersClusterMngmtTabsWindow::removeUsersfromCluster(QString hostname, QString usernamepassword, QString version, QListWidget *list1, QListWidget *list2, QStatusBar * statusbar) {
//    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
//    progbar.show();
    int goodAdditions = 0, badAdditions = 0, totalUpdates = list1->selectedItems().count();
    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString;
        if (version != "8.x") {
        jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 'f' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        } else {
            jsonString = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
            jsonString += "<SOAP-ENV:Body><axl:executeSQLQuery xmlns:axl=\"http://www.cisco.com/AXL/7.0\" xsi:schemaLocation=\"http://www.cisco.com/AXL/1.0 http://gkar.cisco.com/schema/axlsoap.xsd\" sequence=\"1234\">";
            jsonString += "<sql>UPDATE enduser SET enduser.islocaluser = 'f' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></axl:executeSQLQuery></SOAP-ENV:Body></SOAP-ENV:Envelope>";
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

        if ( !statusCode.isValid() ) {
            badAdditions++;
        }

        int status = statusCode.toInt();

        if ( status != 200 ) {
            QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
           appendToFile("Failed to connect to " + hostname.toLocal8Bit() + " for add user update on userid: " + userid.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
        } else {
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(170, 0, 0));//Correct the color scheme
                goodAdditions++;
                appendToFile("Successfully removed userid " + userid.toLocal8Bit() + " from cluster " + hostname.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
            } else {
                badAdditions++;
                appendToFile("Failed to remove userid " + userid.toLocal8Bit() + " from cluster " + hostname.toLocal8Bit(), QDir::homePath() + "/XIPE/Cluster\ Mngmt/logs", "log_" + Variables::logTime + ".txt");
            }
        }
        statusbar->showMessage("Successful Updates: " + QString::number(goodAdditions) + " | Failed Updates: " + QString::number(badAdditions) + " | Progress: " + QString::number(badAdditions + goodAdditions) + "/" + QString::number(totalUpdates) + " | " + QString::number(totalUpdates - (goodAdditions + badAdditions)) + " left");
    }
    }
}

void EndusersClusterMngmtTabsWindow::on_btnFindUsers_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    int selectedType = ui->comboBoxType->currentIndex();
    QString selectedParameter = ui->comboBoxParameter->currentText();
    QString argumentData = ui->lineEditQueryData->text();
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        list->clear();
        list2->clear();
        findUsers(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], Variables::clusterVersionsF[i], selectedType, selectedParameter, argumentData, list, list2, i);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
    QString *string = messageString[ui->tabWidgetClusters->currentIndex()];
        ui->statusbar->showMessage(QString::fromLocal8Bit(string->toLocal8Bit()));
}

void EndusersClusterMngmtTabsWindow::on_comboBoxType_activated(int index)
{
    QStringList queryParameter12, queryParameter22;
    queryParameter12 << "Equals" << "Contains" << "Begins with" << "Ends with" << "Is Empty" << "Is not Empty";
    queryParameter22 << "True" << "False";
    if (index == 4) {
        ui->comboBoxParameter->clear();
        ui->comboBoxParameter->addItems(queryParameter22);
    } else {
        if (ui->comboBoxParameter->itemText(1) != queryParameter12[1]) {
            ui->comboBoxParameter->clear();
            ui->comboBoxParameter->addItems(queryParameter12);
        }
    }
}

void EndusersClusterMngmtTabsWindow::on_btnAddEndusersToCluster_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        addUsersToCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], Variables::clusterVersionsF[i], list, list2, ui->statusbar);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
}

void EndusersClusterMngmtTabsWindow::on_btnRemoveEndusersFromCluster_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList2[i];
        QListWidget * list2 = myList[i];
        removeUsersfromCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], Variables::clusterVersionsF[i], list, list2, ui->statusbar);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
}

void EndusersClusterMngmtTabsWindow::on_tabWidgetClusters_currentChanged(int index)
{
    //change the messages
        QString *string = messageString[index];
            ui->statusbar->showMessage(QString::fromLocal8Bit(string->toLocal8Bit()));
}



void EndusersClusterMngmtTabsWindow::on_lineEditQueryData_returnPressed()
{
    EndusersClusterMngmtTabsWindow::on_btnFindUsers_clicked();
}

void EndusersClusterMngmtTabsWindow::on_actionBack_to_Cluster_List_Interface_triggered()
{
    EndusersClusterMngmtMainWindow * window = new EndusersClusterMngmtMainWindow();
    window->show();
    this->close();
}

void EndusersClusterMngmtTabsWindow::on_actionExit_triggered()
{
    this->close();
}

void EndusersClusterMngmtTabsWindow::on_actionAdd_Selected_Endusers_to_Cluster_triggered()
{
    on_btnAddEndusersToCluster_clicked();
}

void EndusersClusterMngmtTabsWindow::on_actionRemove_Selected_Endusers_from_Cluster_triggered()
{
    on_btnRemoveEndusersFromCluster_clicked();
}

void EndusersClusterMngmtTabsWindow::on_actionLog_Interface_triggered()
{
    if (Variables::logInterfaceShowing == true) {
        Variables::logInterfaceShowing = false;
        Variables::logInterface->hide();
    } else {
        Variables::logInterfaceShowing = true;
        Variables::logInterface->show();
    }
}

void EndusersClusterMngmtTabsWindow::on_actionLog_Interface_changed()
{

}
