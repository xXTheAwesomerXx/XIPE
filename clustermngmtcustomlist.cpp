#include "clustermngmtcustomlist.h"
#include <QKeyEvent>
#include <QDebug>
#include "variables.h"
#include <QDir>
using namespace Variables;
ClusterMngmtCustomList::ClusterMngmtCustomList()
{

}
void ClusterMngmtCustomList::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        //TODO: Make sure to check if an item is checked before you delete it
        for (int i = 0; i < this->count(); i++) {
            if (this->item(i)->isSelected()) {
            QFile f(QDir::homePath() + "/XIPE/ClusterMngmt/test.txt");
            if(f.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                QString s;
                QTextStream t(&f);
                while(!t.atEnd())
                {
                    QString line = t.readLine();
                    if(!line.contains("<cluster><name>" + Variables::clusterNames[i] + "</name><host>" + Variables::hostNames[i] + "</host><unpwd>" + Variables::usernamePasswords[i] + "</unpwd></cluster>"))
                        s.append(line + "\n");
                }
                f.resize(0);
                t << s;
                f.close();
            }
            if (this->item(i)->checkState() == Qt::Checked) {
                Variables::clusterNamesF.removeOne(Variables::clusterNames[i]);
                Variables::hostNamesF.removeOne(Variables::hostNames[i]);
                Variables::usernamePasswordsF.removeOne(Variables::usernamePasswords[i]);
            }
            Variables::clusterNames.remove(i);
            Variables::hostNames.remove(i);
            Variables::usernamePasswords.remove(i);
            delete this->item(i);
            }
        }
    } else {
        QListWidget::keyPressEvent(event);
    }
}
