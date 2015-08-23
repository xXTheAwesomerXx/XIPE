#ifndef UCCXMIGRATIONTAB_H
#define UCCXMIGRATIONTAB_H

#include <QWidget>
#include <QTableWidget>

namespace Ui {
class UCCXMigrationTab;
}

class UCCXMigrationTab : public QWidget
{
    Q_OBJECT

public:
    explicit UCCXMigrationTab(QWidget *parent = 0);
    ~UCCXMigrationTab();
    QTableWidget *tableWidget;

private:
    Ui::UCCXMigrationTab *ui;
};

#endif // UCCXMIGRATIONTAB_H
