#ifndef UCCXDEFAULTSDIALOG_H
#define UCCXDEFAULTSDIALOG_H

#include <QDialog>

namespace Ui {
class UCCXDefaultsDialog;
}

class UCCXDefaultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UCCXDefaultsDialog(QWidget *parent = 0);
    ~UCCXDefaultsDialog();
    void setPrimarySupervisorIndex(int index);

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_checkBox_clicked();

private:
    Ui::UCCXDefaultsDialog *ui;
};

#endif // UCCXDEFAULTSDIALOG_H
