#ifndef CLUSTERMNGMTCUSTOMLIST_H
#define CLUSTERMNGMTCUSTOMLIST_H
#include <QListWidget>

class ClusterMngmtCustomList : public QListWidget {
    Q_OBJECT
    protected:
    virtual void keyPressEvent(QKeyEvent*);
public:
    ClusterMngmtCustomList();
};

#endif // CLUSTERMNGMTCUSTOMLIST_H
