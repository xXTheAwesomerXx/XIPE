#include "endusersclustermngmttabs.h"
#include "ui_endusersclustermngmttabs.h"
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
using namespace Variables;
QVector<QListWidget*> myList;
QVector<QListWidget*> myList2;
QVector<QString*> messageString;
EndusersClusterMngmtTabs::EndusersClusterMngmtTabs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndusersClusterMngmtTabs)
{
    ui->setupUi(this);
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

EndusersClusterMngmtTabs::~EndusersClusterMngmtTabs()
{
    delete ui;
}

QString EndusersClusterMngmtTabs::getSubstringBetween(QString src, QString start, QString stop) {
    std::string sourceStd = src.toStdString();
    std::string startStd = start.toStdString();
    std::string stopStd = stop.toStdString();
    unsigned first = sourceStd.find(startStd);
    unsigned last = sourceStd.find(stopStd);
    return QString::fromStdString(sourceStd.substr(first, last - first).erase(0, start.length()));
}

void EndusersClusterMngmtTabs::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void EndusersClusterMngmtTabs::onError(QNetworkReply::NetworkError rep) {
    qDebug() << "We got an error, quit!";
}

void EndusersClusterMngmtTabs::findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2, int itemIndex) {
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

//    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
//    progbar.show();
    QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
    if (condition1 == 4) {
        jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where "
                + condition1String + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    } else {
    jsonString += "<soapenv:Body><ns:executeSQLQuery><sql>SELECT enduser.userid,enduser.firstname,enduser.middlename,enduser.lastname,enduser.islocaluser,enduser.telephonenumber from enduser where lower("
            + condition1String + ") " + condition2String + "</sql></ns:executeSQLQuery></SOAP-ENV:Envelope>";
    }
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

    if ( !statusCode.isValid() ) {
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
    } else {
        //qDebug() << response;
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
    }
}

void EndusersClusterMngmtTabs::addUsersToCluster(QString hostname, QString usernamepassword, QListWidget *list1, QListWidget *list2, QLabel *messageLabel) {
//    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
//    progbar.show();
    int goodAdditions = 0, badAdditions = 0, totalUpdates = list1->selectedItems().count();
    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 't' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
        QNetworkRequest request(req);

        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLUpdate\"");
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
        } else {
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(0, 170, 255));//Correct the color scheme
                goodAdditions++;
            } else {
                badAdditions++;
            }
        }
        }
        messageLabel->setText("Successful Updates: " + QString::number(goodAdditions) + " Failed Updates: " + QString::number(badAdditions) + ", " + QString::number(totalUpdates - (badAdditions + goodAdditions)) + " updates left!");
    }
}

void EndusersClusterMngmtTabs::removeUsersfromCluster(QString hostname, QString usernamepassword, QListWidget *list1, QListWidget *list2, QLabel *messageLabel) {
//    ProgressDialog progbar("CMClusters - Connecting...", "Please wait while a connection to the selected clusters are established!");
//    progbar.show();
    int goodAdditions = 0, badAdditions = 0, totalUpdates = list1->selectedItems().count();
    for (int i = list1->count(); i --> 0;) {
        if (list1->item(i)->isSelected()) {
        QString userid = getSubstringBetween(list1->item(i)->text(), QString("("), QString(")"));
        QListWidgetItem* userToRemove = list1->item(i);
        QListWidgetItem* userToAdd = new QListWidgetItem(list1->item(i)->text());
        QByteArray jsonString = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns=\"http://www.cisco.com/AXL/API/8.5\">";
        jsonString += "<soapenv:Body><ns:executeSQLUpdate><sql>UPDATE enduser SET enduser.islocaluser = 'f' WHERE enduser.userid = '" + userid.toLocal8Bit() + "'</sql></ns:executeSQLUpdate></SOAP-ENV:Envelope>";
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        QUrl req("https://" + hostname.toLocal8Bit() + ":8443/axl/");
        QNetworkRequest request(req);

        request.setRawHeader("SOAPAction", "\"CUCM:DB ver=8.5 executeSQLUpdate\"");
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
        } else {
            if (response.contains("<rowsUpdated>1</rowsUpdated>")) {
                delete userToRemove;
                list2->addItem(userToAdd);
                userToAdd->setBackground(QColor(170, 0, 0));//Correct the color scheme
                goodAdditions++;
            } else {
                badAdditions++;
            }
        }
    }
        messageLabel->setText("Successful Updates: " + QString::number(goodAdditions) + " Failed Updates: " + QString::number(badAdditions) + ", " + QString::number(totalUpdates - (badAdditions + goodAdditions)) + " updates left!");
    }
}

void EndusersClusterMngmtTabs::on_btnFindUsers_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    for (int i = 0; i < Variables::clusterNamesF.count(); i++) {
    }
    int selectedType = ui->comboBoxType->currentIndex();
    QString selectedParameter = ui->comboBoxParameter->currentText();
    QString argumentData = ui->lineEditQueryData->text();
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        list->clear();
        list2->clear();
        findUsers(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], selectedType, selectedParameter, argumentData, list, list2, i);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
    QString *string = messageString[ui->tabWidgetClusters->currentIndex()];
        ui->labelEndusersInfoMessage->setText(QString::fromLocal8Bit(string->toLocal8Bit()));
}

void EndusersClusterMngmtTabs::on_comboBoxType_activated(int index)
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

void EndusersClusterMngmtTabs::on_btnAddEndusersToCluster_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList[i];
        QListWidget * list2 = myList2[i];
        addUsersToCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], list, list2, ui->labelEndusersInfoMessage);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
}

void EndusersClusterMngmtTabs::on_btnRemoveEndusersFromCluster_clicked()
{
    ui->btnFindUsers->setEnabled(false);
    ui->btnAddEndusersToCluster->setEnabled(false);
    ui->btnRemoveEndusersFromCluster->setEnabled(false);
    for (int i = 0; i < myList.count(); i++) {
        // XXX: use this to update all lists, after getting their list data from getting the endusers
        QListWidget * list = myList2[i];
        QListWidget * list2 = myList[i];
        removeUsersfromCluster(Variables::hostNamesF[i], Variables::usernamePasswordsF[i], list, list2, ui->labelEndusersInfoMessage);
    }
    ui->btnFindUsers->setEnabled(true);
    ui->btnAddEndusersToCluster->setEnabled(true);
    ui->btnRemoveEndusersFromCluster->setEnabled(true);
}

void EndusersClusterMngmtTabs::on_tabWidgetClusters_currentChanged(int index)
{
    //change the messages
        QString *string = messageString[index];
            ui->labelEndusersInfoMessage->setText(QString::fromLocal8Bit(string->toLocal8Bit()));
}
