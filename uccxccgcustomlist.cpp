#include "uccxccgcustomlist.h"
#include "variables.h"
#include <QKeyEvent>
#include <QDebug>

using namespace Variables;
UCCXCCGCustomList::UCCXCCGCustomList()
{

}

void UCCXCCGCustomList::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        for (int i = this->count(); i --> 0;) {
            if (this->item(i)->isSelected()) {
                Variables::uccxHostCCGNameMap.remove(i);
                Variables::uccxClientCCGNameMap.remove(i);
                Variables::uccxHostCCGLinkMap.remove(i);
                Variables::uccxClientCCGLinkMap.remove(i);
                Variables::uccxHostCCGIDMap.remove(i);
                Variables::uccxClientCCGIDMap.remove(i);
                delete this->item(i);
            }
        }
    } else {
        QListWidget::keyPressEvent(event);
    }
}
