#ifndef ENDUSERSCLUSTERMNGMT_H
#define ENDUSERSCLUSTERMNGMT_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QLabel>

namespace Ui {
class EndusersClusterMngmt;
}

class EndusersClusterMngmt : public QDialog
{
    Q_OBJECT

public:
    explicit EndusersClusterMngmt(QWidget *parent = 0);
    ~EndusersClusterMngmt();

public slots:
    bool addConnection(QString, QString, QLabel * label, QPushButton * button);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);

private slots:
    void on_pushButtonProceedToMngmt_clicked();

    void on_btnAddCluster_clicked();

    void on_pushButtonTestConnections_clicked();

private:
    Ui::EndusersClusterMngmt *ui;
};

#endif // ENDUSERSCLUSTERMNGMT_H
