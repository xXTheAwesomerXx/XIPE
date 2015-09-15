#ifndef ENDUSERSCLUSTERMNGMTTABSWINDOW_H
#define ENDUSERSCLUSTERMNGMTTABSWINDOW_H

#include <QMainWindow>
#include "endusersclustermngmtmainwindow.h"
#include <QListWidget>
namespace Ui {
class EndusersClusterMngmtTabsWindow;
}

class EndusersClusterMngmtTabsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EndusersClusterMngmtTabsWindow(QWidget *parent = 0);
    ~EndusersClusterMngmtTabsWindow();

private slots:
    QString getSubstringBetween(QString src, QString start, QString stop);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    void findUsers(QString hostname, QString usernamepassword, int condition1, QString condition2, QString argument, QListWidget * list, QListWidget * list2, int listIndex);
    void addUsersToCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2, QStatusBar * statusbar);
    void removeUsersfromCluster(QString hostname, QString usernamepassword, QListWidget * list1, QListWidget * list2, QStatusBar * statusbar);
    void on_btnFindUsers_clicked();
    void on_comboBoxType_activated(int index);

    void on_btnAddEndusersToCluster_clicked();

    void on_btnRemoveEndusersFromCluster_clicked();

    void on_tabWidgetClusters_currentChanged(int index);

    void on_lineEditQueryData_returnPressed();
    void appendToFile(QString text, QString filePath, QString fileName);
    void on_actionBack_to_Cluster_List_Interface_triggered();

    void on_actionExit_triggered();

    void on_actionAdd_Selected_Endusers_to_Cluster_triggered();

    void on_actionRemove_Selected_Endusers_from_Cluster_triggered();

private:
    Ui::EndusersClusterMngmtTabsWindow *ui;
};

#endif // ENDUSERSCLUSTERMNGMTTABSWINDOW_H
