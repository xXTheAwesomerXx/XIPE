#ifndef LOGINTERFACEDIALOG_H
#define LOGINTERFACEDIALOG_H

#include <QDialog>
#include "QTextBrowser"

namespace Ui {
class LogInterfaceDialog;
}

class LogInterfaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogInterfaceDialog(QWidget *parent = 0);
    virtual void closeEvent(QCloseEvent *event);
    ~LogInterfaceDialog();
    void setLogText(QString text);

private slots:
    void on_textBrowser_textChanged();

private:
    Ui::LogInterfaceDialog *ui;
};

#endif // LOGINTERFACEDIALOG_H
