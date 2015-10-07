#include "uccxmigrationmainwindow.h"
#include "ui_uccxmigrationmainwindow.h"
#include "uccxtabbedwindow.h"
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
#include "QScrollBar"

QString base64_encode(QString string);
QString base64_decode(QString string);
UCCXMigrationMainWindow::UCCXMigrationMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UCCXMigrationMainWindow)
{
    ui->setupUi(this);
    Variables::uccxClientConnected = false;
    Variables::uccxHostConnected = false;
    Variables::uccxPushCCGs = false;
    Variables::replacePrimarySupervisor = false;
    Variables::uccxMappedCCGs = false;
    Variables::uccxHostIP.clear();
    Variables::uccxHostUsernamePwd.clear();
    Variables::uccxClientIP.clear();
    Variables::uccxClientUsernamePwd.clear();
    Variables::defaultPrimarySupName.clear();
    Variables::defaultPrimarySupRefURL.clear();
        Variables::uccxDefaultPrimarySupervisorName.clear();
        Variables::uccxDefaultPrimarySupervisorLink.clear();
        Variables::uccxDefaultPrimarySupervisorNameList.clear();
        Variables::uccxDefaultPrimarySupervisorLinkList.clear();
       Variables::uccxHostCCGIDs.clear();
       Variables::uccxClientCCGIDs.clear();
       Variables::uccxHostCCGNames.clear();
       Variables::uccxClientCCGNames.clear();
       Variables::uccxHostCCGLinks.clear();
       Variables::uccxClientCCGLinks.clear();
       Variables::uccxHostCCGIDMap.clear();
       Variables::uccxClientCCGIDMap.clear();
       Variables::uccxHostCCGNameMap.clear();
       Variables::uccxClientCCGNameMap.clear();
       Variables::uccxHostCCGLinkMap.clear();
       Variables::uccxClientCCGLinkMap.clear();
    if (Variables::logInterfaceShowing == true) {
        ui->actionLog_Interface->setChecked(true);
        qDebug() << "Setting log checked to true, because it's showing...";
    }
    QDir appDir;
    appendToFile("Started UCCX Migration Utility", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Skills")) {
            appendToFile("Successfully created Skills directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Resource\ Groups")) {
            appendToFile("Successfully created Resource Groups directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Applications")) {
            appendToFile("Successully created Applications directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/CSQs")) {
            appendToFile("Successully created CSQs directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Teams")) {
            appendToFile("Successully created Teams directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
    if (appDir.mkpath(QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/Triggers")) {
            appendToFile("Successully created Triggers directory", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
    }
}

UCCXMigrationMainWindow::~UCCXMigrationMainWindow()
{
    delete ui;
}

void UCCXMigrationMainWindow::appendToFile(QString text, QString filePath, QString fileName) {
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

bool UCCXMigrationMainWindow::testConnection(QString hostname, QString usernamepassword, QStatusBar * statusbar) {
    statusbar->showMessage("Connecting to " + hostname.toLocal8Bit() + " please wait!");
    appendToFile("Connecting to " + hostname.toLocal8Bit() + " please wait!", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
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
        statusbar->showMessage("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray());
        appendToFile("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray(), QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
        return false;
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        statusbar->showMessage("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray());
        appendToFile("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray(), QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
        return false;
    } else {
        statusbar->showMessage("Successfully connected to " + hostname.toLocal8Bit() + "!");
        appendToFile("Successfully connected to " + hostname.toLocal8Bit() + "!", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
        return true;
    }
    return false;
}

void UCCXMigrationMainWindow::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    QMessageBox::information(this,"sal",str,"ok");

}

void UCCXMigrationMainWindow::onError(QNetworkReply::NetworkError rep) {
    appendToFile("Encountered an error while attempting to establish a secure connection! Retrying...", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
}

void UCCXMigrationMainWindow::on_pushButtonTestHost_clicked()
{
    ui->pushButtonTestHost->setEnabled(false);
    if (testConnection(ui->lineEditHostIP->text(), base64_encode(ui->lineEditHostUsername->text() + QString::fromStdString(":") + ui->lineEditHostPassword->text()), ui->statusbar)) {
        ui->lineEditHostIP->setEnabled(false);
        ui->lineEditHostPassword->setEnabled(false);
        ui->lineEditHostUsername->setEnabled(false);
        Variables::uccxHostIP = ui->lineEditHostIP->text();
        Variables::uccxHostUsernamePwd = base64_encode(ui->lineEditHostUsername->text() + QString::fromStdString(":") + ui->lineEditHostPassword->text());
        Variables::uccxHostConnected = true;
    } else {
        ui->pushButtonTestHost->setEnabled(true);
    }
}

void UCCXMigrationMainWindow::on_pushButtonTestClient_clicked()
{
    ui->pushButtonTestClient->setEnabled(false);
    if (testConnection(ui->lineEditClientIP->text(), base64_encode(ui->lineEditClientUsername->text() + QString::fromStdString(":") + ui->lineEditClientPassword->text()), ui->statusbar)) {
        ui->lineEditClientIP->setEnabled(false);
        ui->lineEditClientPassword->setEnabled(false);
        ui->lineEditClientUsername->setEnabled(false);
        Variables::uccxClientIP = ui->lineEditClientIP->text();
        Variables::uccxClientUsernamePwd = base64_encode(ui->lineEditClientUsername->text() + QString::fromStdString(":") + ui->lineEditClientPassword->text());
        Variables::uccxClientConnected = true;
    } else {
        ui->pushButtonTestClient->setEnabled(true);
    }
}

void UCCXMigrationMainWindow::on_pushButtonProceed_clicked()
{
    qDebug() << "Proceeding...";
    if (Variables::uccxClientConnected == false || Variables::uccxHostConnected == false) {
        qDebug() << "Both are false?";
        QMessageBox::critical(this, "UCCXMigrator - Error", "Verify that a host and client server is reachable and try again!");
    } else {
        qDebug() << "We good, proceed.";
        UCCXTabbedWindow * tabbedMainWindow = new UCCXTabbedWindow();
        tabbedMainWindow->show();
        delete this;
    }
}

void UCCXMigrationMainWindow::on_actionBack_to_Main_Window_triggered()
{
    MainWindow * window = new MainWindow();
    window->show();
    delete this;
}

void UCCXMigrationMainWindow::on_actionExit_triggered()
{
    this->close();
}

void UCCXMigrationMainWindow::on_actionLog_Interface_triggered()
{
    if (Variables::logInterfaceShowing == true) {
        Variables::logInterfaceShowing = false;
        Variables::logInterface->hide();
        qDebug() << "Hiding Log";
    } else {
        Variables::logInterfaceShowing = true;
        Variables::logInterface->show();
        qDebug() << "Showing Log";
    }
}
