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
    void on_pushButton_clicked();
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
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);

private:
    Ui::UCCXTabbedWindow *ui;
    enum COLUMNS {
        NAME, TEAMID, PRIMARYSUP
    };
};

#endif // UCCXTABBEDWINDOW_H
