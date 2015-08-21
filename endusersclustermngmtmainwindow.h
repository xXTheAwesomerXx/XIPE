#ifndef ENDUSERSCLUSTERMNGMTMAINWINDOW_H
#define ENDUSERSCLUSTERMNGMTMAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QLabel>

namespace Ui {
class EndusersClusterMngmtMainWindow;
}

class EndusersClusterMngmtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EndusersClusterMngmtMainWindow(QWidget *parent = 0);
    ~EndusersClusterMngmtMainWindow();

public slots:
    bool addConnection(QString, QString, QStatusBar * statusbar, QPushButton * button);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);

private slots:
    void on_pushButtonProceedToMngmt_clicked();

    void on_btnAddCluster_clicked();

    void on_pushButtonTestConnections_clicked();

    void on_lineEditClustername_returnPressed();

    void on_lineEditHostname_returnPressed();

    void on_lineEditUsername_returnPressed();

    void on_lineEditPassword_returnPressed();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::EndusersClusterMngmtMainWindow *ui;
};

#endif // ENDUSERSCLUSTERMNGMTMAINWINDOW_H
