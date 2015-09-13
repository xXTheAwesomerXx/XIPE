#ifndef UCCXTABBEDWINDOW_H
#define UCCXTABBEDWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QLabel>

namespace Ui {
class UCCXTabbedWindow;
}

class UCCXTabbedWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UCCXTabbedWindow(QWidget *parent = 0);
    ~UCCXTabbedWindow();

private slots:
    bool getAllTeamData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedTeamData(QString refURL, QString usernamepassword);
    bool getAllAppData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedAppData(QString refURL, QString usernamepassword);
    bool getAllSkillData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedSkillData(QString refURL, QString usernamepassword);
    bool getAllRGData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedRGData(QString refURL, QString usernamepassword);
    bool getAllCSQData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedCSQData(QString refURL, QString usernamepassword);
    bool getAllTriggerData(QString hostname, QString usernamepassword, QString filePath);
    void getDetailedTriggerData(QString refURL, QString usernamepassword);
    bool getAllClientResources(QString hostname, QString usernamepassword);
    bool getAllClientCCGs(QString hostname, QString usernamepassword);
    bool getAllHostCCGs(QString hostname, QString usernamepassword);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);

    void on_btnPushData_clicked();

    void on_btnGetData_clicked();

    void on_btnSelectAll_clicked();

    void on_btnUncheckAll_clicked();
    void writeToLogAndStatus(QString text);
    void appendToFile(QString text, QString filePath, QString fileName);
private:
    Ui::UCCXTabbedWindow *ui;
    enum COLUMNS {
        NAME, TEAMID, PRIMARYSUP
    };
};

#endif // UCCXTABBEDWINDOW_H
