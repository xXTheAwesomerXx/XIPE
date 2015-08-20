#ifndef FINDUSERSMNGMT_H
#define FINDUSERSMNGMT_H

#include <QDialog>

namespace Ui {
class FindUsersMngmt;
}

class FindUsersMngmt : public QDialog
{
    Q_OBJECT

public:
    explicit FindUsersMngmt(QWidget *parent = 0);
    ~FindUsersMngmt();

private:
    Ui::FindUsersMngmt *ui;
};

#endif // FINDUSERSMNGMT_H
