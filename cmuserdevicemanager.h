#ifndef CMUSERDEVICEMANAGER_H
#define CMUSERDEVICEMANAGER_H

#include <QMainWindow>

namespace Ui {
class CMUserDeviceManager;
}

class CMUserDeviceManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMUserDeviceManager(QWidget *parent = 0);
    ~CMUserDeviceManager();

private slots:
    void on_actionBack_to_Cluster_List_triggered();

    void on_actionExit_triggered();

    void on_actionLog_Interface_triggered();

private:
    Ui::CMUserDeviceManager *ui;
};

#endif // CMUSERDEVICEMANAGER_H
