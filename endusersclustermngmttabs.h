#ifndef ENDUSERSCLUSTERMNGMTTABS_H
#define ENDUSERSCLUSTERMNGMTTABS_H

#include <QDialog>
#include "endusersclustermngmt.h"
#include <QListWidget>

namespace Ui {
class EndusersClusterMngmtTabs;
}

class EndusersClusterMngmtTabs : public QDialog
{
    Q_OBJECT

public:
    explicit EndusersClusterMngmtTabs(QWidget *parent = 0);
    ~EndusersClusterMngmtTabs();

private slots:
    QString getSubstringBetween(QString src, QString start, QString stop);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    void findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2, int listIndex);
    void addUsersToCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2, QLabel * messageLabel);
    void removeUsersfromCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2, QLabel * messageLabel);
    void on_btnFindUsers_clicked();
    void on_comboBoxType_activated(int index);

    void on_btnAddEndusersToCluster_clicked();

    void on_btnRemoveEndusersFromCluster_clicked();

    void on_tabWidgetClusters_currentChanged(int index);

private:
    Ui::EndusersClusterMngmtTabs *ui;
};

#endif // ENDUSERSCLUSTERMNGMTTABS_H
