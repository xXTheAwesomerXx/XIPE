#include <string>
#include "variables.h"
#include <QList>
#include <QListWidget>
#include <QDateTime>
namespace Variables
{
   QVector<QString> clusterNames;
   QVector<QString> hostNames;
   QVector<QString> usernamePasswords;
   QVector<QString> clusterNamesF;
   QVector<QString> hostNamesF;
   QVector<QString> usernamePasswordsF;
   QString uccxHostIP, uccxHostUsernamePwd, uccxClientIP, uccxClientUsernamePwd, defaultPrimarySupName, defaultPrimarySupRefURL;
   QString logTime = QString::number(QDateTime::currentMSecsSinceEpoch());
}

