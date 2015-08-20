#ifndef FINDUSERSMAINWINDOW_H
#define FINDUSERSMAINWINDOW_H

#include <QDialog>

namespace Ui {
class FindUsersMainWindow;
}

class FindUsersMainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FindUsersMainWindow(QWidget *parent = 0);
    ~FindUsersMainWindow();

private:
    Ui::FindUsersMainWindow *ui;
};

#endif // FINDUSERSMAINWINDOW_H
