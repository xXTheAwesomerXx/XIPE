#include "clustermngmtcustomlist.h"
#include <QKeyEvent>
#include <QDebug>
#include "variables.h"
#include <QDir>
#include "endusersclustermngmtmainwindow.h"
using namespace Variables;
QString base64_encode(QString string);
QString base64_decode(QString string);
ClusterMngmtCustomList::ClusterMngmtCustomList()
{

}

void ClusterMngmtCustomList::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        //TODO: Make sure to check if an item is checked before you delete it
        for (int i = this->count(); i --> 0;) {
            if (this->item(i)->isSelected()) {
            QFile f(QDir::homePath() + "/XIPE/Cluster\ Mngmt/conn.conf");
            if(f.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                QString s;
                QTextStream t(&f);
                while(!t.atEnd())
                {
                    QString line = t.readLine();
                    if(!line.contains(base64_encode("<cluster><name>" + Variables::clusterNames[i].toLocal8Bit() + "</name><host>" + Variables::hostNames[i].toLocal8Bit() + "</host><unpwd>" + Variables::usernamePasswords[i].toLocal8Bit() + "</unpwd></cluster>"))) {
                        s.append(line + "\n");
                    } else {
                        EndusersClusterMngmtMainWindow window;
                        window.setStatusBarMessage("Removed cluster: " + Variables::clusterNames[i] + " from configuration!");
                    }
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
