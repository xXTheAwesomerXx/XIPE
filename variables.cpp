#include <string>
#include "variables.h"
#include <QList>
#include <QListWidget>
#include <QDateTime>
namespace Variables
{
   LogInterfaceDialog * logInterface;
   QVector<QString> clusterNames;
   QVector<QString> hostNames;
   QVector<QString> usernamePasswords;
   QVector<QString> clusterVersions;
   QVector<QString> clusterNamesF;
   QVector<QString> hostNamesF;
   QVector<QString> usernamePasswordsF;
   QVector<QString> clusterVersionsF;
   QScrollBar * logScrollBar;
   QString uccxHostIP, uccxHostUsernamePwd, uccxClientIP, uccxClientUsernamePwd, defaultPrimarySupName, defaultPrimarySupRefURL;
   QString logTime = QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString().replace(":", "-");
   bool logInterfaceShowing = false, logInterfaceDone = false, uccxClientConnected = false, uccxHostConnected = false, uccxPushCCGs = false, replacePrimarySupervisor = false, uccxMappedCCGs = false;
   QString uccxDefaultPrimarySupervisorName, uccxDefaultPrimarySupervisorLink;
   QStringList uccxDefaultPrimarySupervisorNameList, uccxDefaultPrimarySupervisorLinkList;
   QVector<QString> uccxHostCCGIDs, uccxClientCCGIDs, uccxHostCCGNames, uccxClientCCGNames, uccxHostCCGLinks, uccxClientCCGLinks, uccxHostCCGIDMap, uccxClientCCGIDMap, uccxHostCCGNameMap, uccxClientCCGNameMap, uccxHostCCGLinkMap, uccxClientCCGLinkMap;
}
