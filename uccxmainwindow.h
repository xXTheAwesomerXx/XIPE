#ifndef UCCXMAINWINDOW_H
#define UCCXMAINWINDOW_H

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QLabel>

namespace Ui {
class UCCXMainWindow;
}

class UCCXMainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UCCXMainWindow(QWidget *parent = 0);
    ~UCCXMainWindow();

public slots:
    bool testConnection(QString, QString, QLabel * label, QPushButton * button1, QPushButton * button2);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);

private slots:
    void on_pushButtonTestHost_clicked();

    void on_pushButtonTestClient_clicked();

private:
    Ui::UCCXMainWindow *ui;
};

#endif // UCCXMAINWINDOW_H
