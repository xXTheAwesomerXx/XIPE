#include "uccxdefaultsdialog.h"
#include "ui_uccxdefaultsdialog.h"
#include "variables.h"
#include "uccxccgcustomlist.h"
#include <QMessageBox>
#include "uccxtabbedwindow.h"
#include <QFile>
#include <QDir>

using namespace Variables;
QString getSubstringBetween(QString src, QString start, QString stop);
void appendToFile(QString text, QString filePath, QString fileName);
UCCXDefaultsDialog::UCCXDefaultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UCCXDefaultsDialog)
{
    ui->setupUi(this);
    //primarySupervisorList << "Select a Primary Supervisor";
    ui->comboboxPrimarySupDefault->addItems(Variables::uccxDefaultPrimarySupervisorNameList);
    if (Variables::uccxPushCCGs == true) {
        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        ui->pushButton->setEnabled(false);
    } else {
        for (int i = 0; i < Variables::uccxClientCCGNames.count(); i++) {
            ui->comboBox->addItem(Variables::uccxClientCCGNames[i]);
        }
        for (int i = 0; i < Variables::uccxHostCCGNames.count(); i++) {
            ui->comboBox_2->addItem(Variables::uccxHostCCGNames[i]);
        }
    }
}

UCCXDefaultsDialog::~UCCXDefaultsDialog()
{
    delete ui;
}

void UCCXDefaultsDialog::closeEvent(QCloseEvent *event) {
    if (ui->checkBox->checkState() == Qt::Checked) {
        QString userid = getSubstringBetween(Variables::uccxDefaultPrimarySupervisorNameList[ui->comboboxPrimarySupDefault->currentIndex()], QString(" ["), QString("]"));
        QString supervisorName = QString(Variables::uccxDefaultPrimarySupervisorNameList[ui->comboboxPrimarySupDefault->currentIndex()]).replace(" [" + userid.toLocal8Bit() + "]", "");
        Variables::uccxDefaultPrimarySupervisorName = supervisorName;
        Variables::uccxDefaultPrimarySupervisorLink = Variables::uccxDefaultPrimarySupervisorLinkList[ui->comboboxPrimarySupDefault->currentIndex()];
        Variables::replacePrimarySupervisor = true;
    }
    if (Variables::uccxPushCCGs == false) {
        if (ui->listWidget->count() <= 0) {
            QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, "XIPE - Confirmation", "You selected not to push Call Control Groups to the Client server, however\nYou did not choose to map any Host CCGs to its respective Client CCG.\n\nIf you continue now, the program will assume that all Call Control Groups\non the Client server matches EXACTLY as the Host Servers Call Control Groups\nWould you still like to proceed?",
                                            QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::Yes) {
                Variables::uccxMappedCCGs = false;
                appendToFile("Enduser chose not to push call control groups, and not to map any host/client call control groups!", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
              } else {
                Variables::uccxMappedCCGs = false;
                UCCXDefaultsDialog window;
                window.setPrimarySupervisorIndex(ui->comboboxPrimarySupDefault->currentIndex());
                window.setModal(true);
                window.exec();
              }
        } else {
            Variables::uccxMappedCCGs = true;
        }
    }
}

QString getSubstringBetween(QString src, QString start, QString stop) {
    std::string sourceStd = src.toStdString();
    std::string startStd = start.toStdString();
    std::string stopStd = stop.toStdString();
    unsigned first = sourceStd.find(startStd);
    unsigned last = sourceStd.find(stopStd);
    return QString::fromStdString(sourceStd.substr(first, last - first).erase(0, start.length()));
}

void UCCXDefaultsDialog::on_buttonBox_accepted()
{

    if (ui->checkBox->checkState() == Qt::Checked) {
        QString userid = getSubstringBetween(Variables::uccxDefaultPrimarySupervisorNameList[ui->comboboxPrimarySupDefault->currentIndex()], QString(" ["), QString("]"));
        QString supervisorName = QString(Variables::uccxDefaultPrimarySupervisorNameList[ui->comboboxPrimarySupDefault->currentIndex()]).replace(" [" + userid.toLocal8Bit() + "]", "");
        Variables::uccxDefaultPrimarySupervisorName = supervisorName;
        Variables::uccxDefaultPrimarySupervisorLink = Variables::uccxDefaultPrimarySupervisorLinkList[ui->comboboxPrimarySupDefault->currentIndex()];
        Variables::replacePrimarySupervisor = true;
    }
    if (Variables::uccxPushCCGs == false) {
        if (ui->listWidget->count() <= 0) {
            QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, "XIPE - Confirmation", "You selected not to push Call Control Groups to the Client server, however\nYou did not choose to map any Host CCGs to its respective Client CCG.\n\nIf you continue now, the program will assume that all Call Control Groups\non the Client server matches EXACTLY as the Host Servers Call Control Groups\nWould you still like to proceed?",
                                            QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::Yes) {
                  Variables::uccxMappedCCGs = false;
                appendToFile("Enduser chose not to push call control groups, and not to map any host/client call control groups!", QDir::homePath() + "/XIPE/UCCX\ Migration/" + Variables::logTime + "/logs", "log.txt");
              } else {
                Variables::uccxMappedCCGs = false;
                UCCXDefaultsDialog window;
                window.setPrimarySupervisorIndex(ui->comboboxPrimarySupDefault->currentIndex());
                window.setModal(true);
                window.exec();
              }
        } else {
            Variables::uccxMappedCCGs = true;
        }
    }
}

void UCCXDefaultsDialog::setPrimarySupervisorIndex(int index) {
    ui->comboboxPrimarySupDefault->setCurrentIndex(index);
}

void UCCXDefaultsDialog::on_pushButton_clicked()
{
    QListWidgetItem *item = new QListWidgetItem("Replacing " + Variables::uccxHostCCGNames[ui->comboBox->currentIndex()].toLocal8Bit() + " with " + Variables::uccxClientCCGNames[ui->comboBox_2->currentIndex()].toLocal8Bit());
    ui->listWidget->addItem(item);
    Variables::uccxHostCCGNameMap.append(Variables::uccxHostCCGNames[ui->comboBox->currentIndex()]);
    Variables::uccxClientCCGNameMap.append(Variables::uccxClientCCGNames[ui->comboBox_2->currentIndex()]);
    Variables::uccxHostCCGLinkMap.append(Variables::uccxHostCCGLinks[ui->comboBox->currentIndex()]);
    Variables::uccxClientCCGLinkMap.append(Variables::uccxClientCCGLinks[ui->comboBox_2->currentIndex()]);
    Variables::uccxHostCCGIDMap.append(Variables::uccxHostCCGIDs[ui->comboBox->currentIndex()]);
    Variables::uccxClientCCGIDMap.append(Variables::uccxClientCCGIDs[ui->comboBox_2->currentIndex()]);
}

void UCCXDefaultsDialog::on_checkBox_clicked()
{
    // enable/disable primary sup. parse
    if (ui->checkBox->checkState() == Qt::Checked) {
        ui->comboboxPrimarySupDefault->setEnabled(true);
    } else {
        ui->comboboxPrimarySupDefault->setEnabled(false);
    }
}
