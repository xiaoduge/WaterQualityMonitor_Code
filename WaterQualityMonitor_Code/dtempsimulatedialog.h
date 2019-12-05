#ifndef DTEMPSIMULATEDIALOG_H
#define DTEMPSIMULATEDIALOG_H

#include <QDialog>

namespace Ui {
class DTempSimulateDialog;
}

class DTempSimulateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DTempSimulateDialog(QWidget *parent = 0);
    ~DTempSimulateDialog();

private slots:
    void on_checkBox_sw_stateChanged(int arg1);

    void on_pushButton_confirm_clicked();

private:
    void initUI();

private:
    Ui::DTempSimulateDialog *ui;
};

#endif // DTEMPSIMULATEDIALOG_H
