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
QString base64_encode(QString string);
QString base64_decode(QString string);
UCCXMigrationMainWindow::UCCXMigrationMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UCCXMigrationMainWindow)
{
    ui->setupUi(this);
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

bool UCCXMigrationMainWindow::testConnection(QString hostname, QString usernamepassword, QStatusBar * statusbar) {
    statusbar->showMessage("Connecting to " + hostname.toLocal8Bit() + " please wait!");
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
        return false;
    }

    int status = statusCode.toInt();

    if ( status != 200 ) {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        statusbar->showMessage("Connection to " + hostname.toLocal8Bit() + " failed! Status Code: " + statusCode.toByteArray());
        return false;
    } else {
        statusbar->showMessage("Successfully connected to " + hostname.toLocal8Bit() + "!");
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
    if (Variables::uccxClientConnected == false || Variables::uccxHostConnected == false) {
         QMessageBox::critical(this, "UCCXMigrator - Error", "Verify that a host and client server is reachable and try again!");
    } else {
        this->hide();
        UCCXTabbedWindow * tabbedMainWindow = new UCCXTabbedWindow();
        tabbedMainWindow->show();
    }
}
