#ifndef CLUSTERTAB_H
#define CLUSTERTAB_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class ClusterTab;
}

class ClusterTab : public QWidget
{
    Q_OBJECT

public:
    explicit ClusterTab(QWidget *parent = 0);
    ~ClusterTab();
    QListWidget *listWidget;
    QListWidget *listWidget2; //Actually the first in the list

private:
    Ui::ClusterTab *ui;
    QString clusterName;
    QString hostName;
    QString usernamePassword;
};

#endif // CLUSTERTAB_H
