#ifndef UCCXCCGCUSTOMLIST_H
#define UCCXCCGCUSTOMLIST_H
#include <QListWidget>

class UCCXCCGCustomList: public QListWidget {
    Q_OBJECT
    protected:
    virtual void keyPressEvent(QKeyEvent*);
public:
    UCCXCCGCustomList();
};

#endif // UCCXCCGCUSTOMLIST_H
