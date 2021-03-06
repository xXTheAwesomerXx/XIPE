#ifndef UCCXMIGRATIONMAINWINDOW_H
#define UCCXMIGRATIONMAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkReply>
#include <QLabel>

namespace Ui {
class UCCXMigrationMainWindow;
}

class UCCXMigrationMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UCCXMigrationMainWindow(QWidget *parent = 0);
    ~UCCXMigrationMainWindow();

public slots:
    bool testConnection(QString, QString, QStatusBar * statusbar);
    void onfinish(QNetworkReply *rep);
    void onError(QNetworkReply::NetworkError reply);
    void appendToFile(QString text, QString filePath, QString fileName);

private slots:
    void on_pushButtonTestHost_clicked();

    void on_pushButtonTestClient_clicked();

    void on_pushButtonProceed_clicked();

    void on_actionBack_to_Main_Window_triggered();

    void on_actionExit_triggered();

    void on_actionLog_Interface_triggered();

private:
    Ui::UCCXMigrationMainWindow *ui;
};

#endif // UCCXMIGRATIONMAINWINDOW_H
