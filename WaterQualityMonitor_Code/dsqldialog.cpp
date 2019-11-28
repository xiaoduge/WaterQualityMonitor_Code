#include "dsqldialog.h"
#include <QSqlTableModel>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include "config.h"
#include <QDebug>
#include <QSqlQuery>
#include <QLineEdit>


DSqlDialog::DSqlDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    init();
}

void DSqlDialog::init()
{
    initUI();
    initSqlTableModel();
    initConnect();
}

void DSqlDialog::initUI()
{
    this->resize(800, 600);
    this->setWindowTitle(tr("Sql Data"));
    m_pTableView = new QTableView;
    m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_pSqlCheckBox = new QCheckBox;
    if(gConfig.openDataLogger)
    {
        m_pSqlCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_pSqlCheckBox->setCheckState(Qt::Unchecked);
    }

    QHBoxLayout *h1Layout = new QHBoxLayout;
    h1Layout->setAlignment(Qt::AlignLeft);
    h1Layout->addWidget(m_pSqlCheckBox);

    QHBoxLayout *h2Layout = new QHBoxLayout;
    h2Layout->setAlignment(Qt::AlignCenter);

    m_pChlComboBox = new QComboBox;
    for(int i = 0; i < Channel_Num; ++i)
    {
        m_pChlComboBox->addItem(QString("Channel %1").arg(i + 1));
    }
    h2Layout->addWidget(m_pChlComboBox);

    for(int i = 0; i < SqlDlgBtn_Num; ++i)
    {
        m_pBtns[i] = new QPushButton;
        m_pBtns[i]->setMinimumWidth(100);
        h2Layout->addWidget(m_pBtns[i]);
    }

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(h1Layout);
    vLayout->addWidget(m_pTableView);
    vLayout->addLayout(h2Layout);

    this->setLayout(vLayout);

    buildTranslation();
}

void DSqlDialog::initSqlTableModel()
{
    m_pTableModel = new QSqlTableModel(this);
}

void DSqlDialog::initConnect()
{
    connect(m_pBtns[SqlQuery_Btn], SIGNAL(clicked()), this, SLOT(on_sqlQueryBtn_clicked()));
    connect(m_pBtns[SqlDelete_Btn], SIGNAL(clicked()), this, SLOT(on_sqlClearBtn_clicked()));
    connect(m_pSqlCheckBox, SIGNAL(stateChanged(int)), this, SLOT(on_sqlCheck_stateChanged(int)));
}

void DSqlDialog::buildTranslation()
{
    m_pBtns[SqlQuery_Btn]->setText(tr("Query"));
    m_pBtns[SqlDelete_Btn]->setText(tr("Delete"));
    m_pSqlCheckBox->setText(tr("Data Logger"));
}

void DSqlDialog::initTabelHeaderData(QSqlTableModel *tableModel)
{
    tableModel->setHeaderData(0, Qt::Horizontal, tr("id"));
    tableModel->setHeaderData(1, Qt::Horizontal, tr("WaterQuality"));
    tableModel->setHeaderData(2, Qt::Horizontal, tr("Temperature"));
    tableModel->setHeaderData(3, Qt::Horizontal, tr("Time"));
}

void DSqlDialog::showEvent(QShowEvent *event)
{
    if(gConfig.openDataLogger)
    {
        m_pSqlCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_pSqlCheckBox->setCheckState(Qt::Unchecked);
    }

    QDialog::showEvent(event);
}

void DSqlDialog::on_sqlQueryBtn_clicked()
{
    qDebug() << "sql query";
    int iChl = m_pChlComboBox->currentIndex();
    switch (iChl)
    {
    case 0:
        m_pTableModel->setTable("Chl1_Data");
        this->setWindowTitle(tr("First Channel Data"));
        break;
    case 1:
        m_pTableModel->setTable("Chl2_Data");
        this->setWindowTitle(tr("Second Channel Data"));
        break;
    case 2:
        m_pTableModel->setTable("Chl3_Data");
        this->setWindowTitle(tr("Third Channel Data"));
        break;
    default:
        break;
    }

    m_pTableModel->select();
    initTabelHeaderData(m_pTableModel);
    m_pTableView->setModel(m_pTableModel);
    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pTableView->scrollToBottom();
}

void DSqlDialog::on_sqlClearBtn_clicked()
{
    QMutexLocker mutexLocker(&gSqlMutex);
    QSqlQuery query;

    bool ret = query.exec("delete from Chl1_Data");
    if(!ret)
    {
        gLogger.log("Sql:delete from Chl1_Data error", __FILE__, __LINE__);
    }

    ret = query.exec("delete from Chl2_Data");
    if(!ret)
    {
        gLogger.log("Sql:delete from Chl2_Data error", __FILE__, __LINE__);
    }

    ret = query.exec("delete from Chl3_Data");
    if(!ret)
    {
        gLogger.log("Sql:delete from Chl3_Data error", __FILE__, __LINE__);
    }
    this->setWindowTitle(tr("All data in the database have been cleared"));
}

void DSqlDialog::on_sqlCheck_stateChanged(int state)
{
    if(Qt::Checked == state)
    {
        gConfig.openDataLogger = true;
    }
    else
    {
        gConfig.openDataLogger = false;
    }
    MainSaveConfigParam();
}
