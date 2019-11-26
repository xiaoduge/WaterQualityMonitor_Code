#include "dhintdialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMutexLocker>
#include <QDebug>
#include "config.h"

DHintDialog *DHintDialog::instance = nullptr;

QMutex hintDialogmutex;

DHintDialog::DHintDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QColor(60, 60,120));
    this->setPalette(palette);

    palette = this->palette();
    palette.setBrush(QPalette::WindowText, Qt::white);
    this->setPalette(palette);//

    setFixedSize(250, 70); //200 60

    m_pLbInfo    = new QLabel;
    m_pLbInfo->setAlignment(Qt::AlignCenter);

    m_pBottomLayout =  new QVBoxLayout;

    m_pBottomLayout->addWidget(m_pLbInfo);

    setLayout(m_pBottomLayout);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timer_event()));
}

DHintDialog::~DHintDialog()
{
    qDebug() << "~DHintDialog()";
}

void DHintDialog::setInfo(QString strText)
{
    m_pLbInfo->setText(strText);
}

void DHintDialog::getInstance(QString strText)
{
    if (nullptr == instance)
    {
        QMutexLocker locker(&hintDialogmutex);
        if(nullptr == instance)
        {
            DHintDialog *pDlg = new DHintDialog();
            pDlg->setInfo(strText);
            pDlg->start();
            instance = pDlg;
        }
    }
}

void DHintDialog::start()
{
    m_timer->start(1000);
    show();
}

void DHintDialog::on_timer_event()
{
    if(instance != nullptr)
    {
        m_timer->stop();
        delete this;
        instance = nullptr;
    }
}
