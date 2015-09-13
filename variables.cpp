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
   bool uccxClientConnected = false, uccxHostConnected = false, uccxPushCCGs = false, replacePrimarySupervisor = false, uccxMappedCCGs = false;
   QString uccxDefaultPrimarySupervisorName, uccxDefaultPrimarySupervisorLink;
   QStringList uccxDefaultPrimarySupervisorNameList, uccxDefaultPrimarySupervisorLinkList;
   QVector<QString> uccxHostCCGIDs, uccxClientCCGIDs, uccxHostCCGNames, uccxClientCCGNames, uccxHostCCGLinks, uccxClientCCGLinks, uccxHostCCGIDMap, uccxClientCCGIDMap, uccxHostCCGNameMap, uccxClientCCGNameMap, uccxHostCCGLinkMap, uccxClientCCGLinkMap;
}
