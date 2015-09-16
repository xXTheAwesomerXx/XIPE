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
    bool addConnection(QString, QString, QString, QStatusBar * statusbar, QPushButton * button);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    void setStatusBarMessage(QString text);

private slots:
    void on_pushButtonProceedToMngmt_clicked();

    void on_btnAddCluster_clicked();

    void on_pushButtonTestConnections_clicked();

    void on_lineEditClustername_returnPressed();

    void on_lineEditHostname_returnPressed();

    void on_lineEditUsername_returnPressed();

    void on_lineEditPassword_returnPressed();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionProduct_Registration_triggered();

    void on_actionRemove_Selected_Clusters_triggered();

    void on_actionBack_To_Main_triggered();

    void on_actionExit_triggered();

    void on_actionLog_Interface_triggered();

    void on_actionAbout_Cluster_Management_triggered();

    void on_actionFeedback_triggered();

    void on_actionReport_a_bug_triggered();

    void on_actionTechnical_Support_triggered();

    void on_actionFAQ_triggered();

    void on_actionView_Help_triggered();

private:
    Ui::EndusersClusterMngmtMainWindow *ui;
};

#endif // ENDUSERSCLUSTERMNGMTMAINWINDOW_H
