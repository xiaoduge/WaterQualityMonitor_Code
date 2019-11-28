#ifndef DSQLDIALOG_H
#define DSQLDIALOG_H

#include <QDialog>

class QTableView;
class QSqlTableModel;
class QPushButton;
class QCheckBox;
class QComboBox;
class QLineEdit;

class DSqlDialog : public QDialog
{
    Q_OBJECT

public:
    enum SqlDlgBtn
    {
        SqlQuery_Btn = 0,
        SqlDelete_Btn,
        SqlDlgBtn_Num
    };

public:
    DSqlDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

private:
    void init();
    void initUI();
    void initSqlTableModel();
    void initConnect();
    void buildTranslation();
    void initTabelHeaderData(QSqlTableModel *tableModel);

protected:
    void showEvent(QShowEvent *event);

protected slots:
    void on_sqlQueryBtn_clicked();
    void on_sqlClearBtn_clicked();
    void on_sqlCheck_stateChanged(int state);

private:
    QTableView* m_pTableView;
    QSqlTableModel* m_pTableModel;

    QComboBox *m_pChlComboBox;
    QCheckBox *m_pSqlCheckBox;
    QPushButton *m_pBtns[SqlDlgBtn_Num];
};

#endif // DSQLDIALOG_H
