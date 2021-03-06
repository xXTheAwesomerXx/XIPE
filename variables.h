#ifndef VARIABLES_H
#define VARIABLES_H
#include <string>
#include <QListWidget>
#include <QVector>
#include "loginterfacedialog.h"

namespace Variables
{
    extern LogInterfaceDialog * logInterface;
    extern QVector<QListWidget*> list; //QList of Pointers

    extern QVector<QString> clusterNames;
    extern QVector<QString> hostNames;
    extern QVector<QString> usernamePasswords;
    extern QVector<QString> clusterVersions;
    extern QVector<QString> clusterNamesF;
    extern QVector<QString> hostNamesF;
    extern QVector<QString> usernamePasswordsF;
    extern QVector<QString> clusterVersionsF;
    extern QScrollBar * logScrollBar;

    extern QString uccxHostIP, uccxHostUsernamePwd, uccxClientIP, uccxClientUsernamePwd, defaultPrimarySupName, defaultPrimarySupRefURL;
    extern QString logTime;
    extern bool logInterfaceShowing, logInterfaceDone, uccxClientConnected, uccxHostConnected, uccxPushCCGs, replacePrimarySupervisor, uccxMappedCCGs;
    extern QString uccxDefaultPrimarySupervisorName, uccxDefaultPrimarySupervisorLink;
    extern QStringList uccxDefaultPrimarySupervisorNameList, uccxDefaultPrimarySupervisorLinkList;
    extern QVector<QString> uccxHostCCGIDs, uccxClientCCGIDs, uccxHostCCGNames, uccxClientCCGNames, uccxHostCCGLinks, uccxClientCCGLinks, uccxHostCCGIDMap, uccxClientCCGIDMap, uccxHostCCGNameMap, uccxClientCCGNameMap, uccxHostCCGLinkMap, uccxClientCCGLinkMap;
}
#endif // VARIABLES_H
