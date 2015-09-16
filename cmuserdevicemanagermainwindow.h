#ifndef CMUSERDEVICEMANAGERMAINWINDOW_H
#define CMUSERDEVICEMANAGERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class CMUserDeviceManagerMainWindow;
}

class CMUserDeviceManagerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMUserDeviceManagerMainWindow(QWidget *parent = 0);
    ~CMUserDeviceManagerMainWindow();

private:
    Ui::CMUserDeviceManagerMainWindow *ui;
};

#endif // CMUSERDEVICEMANAGERMAINWINDOW_H
