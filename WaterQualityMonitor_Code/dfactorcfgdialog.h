#ifndef DFACTORCFGDIALOG_H
#define DFACTORCFGDIALOG_H

#include <QDialog>

namespace Ui {
class DFactorCfgDialog;
}

class DFactorCfgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DFactorCfgDialog(QWidget *parent = 0);
    ~DFactorCfgDialog();

signals:
    void factor(const float fac);

private slots:
    void on_pushButton_cfg_clicked();

    void on_pushButton_read_clicked();

private:
    void initUI();

private:
    Ui::DFactorCfgDialog *ui;
};

#endif // DFACTORCFGDIALOG_H
