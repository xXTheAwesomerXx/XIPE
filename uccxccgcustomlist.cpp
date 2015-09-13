#include "uccxccgcustomlist.h"
#include "variables.h"
#include <QKeyEvent>

using namespace Variables;
UCCXCCGCustomList::UCCXCCGCustomList()
{

}

void UCCXCCGCustomList::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        //qDebug() << "Delete key was pressed!";
    } else {
        QListWidget::keyPressEvent(event);
    }
}
