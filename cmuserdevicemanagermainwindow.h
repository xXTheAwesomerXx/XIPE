#ifndef CMUSERDEVICEMANAGERMAINWINDOW_H
#define CMUSERDEVICEMANAGERMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class CMUserDeviceManagerMainWindow;
}

class CMUserDeviceManagerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMUserDeviceManagerMainWindow(QWidget *parent = 0);
    ~CMUserDeviceManagerMainWindow();
    void setStatusBarMessage(QString text);

private slots:
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    bool addConnection(QString hostname, QString usernamepassword, QString version, QStatusBar * statusbar, QPushButton * button);
    void on_actionLog_Interface_triggered();

    void on_actionRemove_Cluster_triggered();

    void on_actionBack_to_Main_Window_triggered();

    void on_actionExit_triggered();

    void on_pushButtonSave_clicked();

    void on_pushButtonConnect_clicked();
    
private:
    Ui::CMUserDeviceManagerMainWindow *ui;
};

#endif // CMUSERDEVICEMANAGERMAINWINDOW_H
