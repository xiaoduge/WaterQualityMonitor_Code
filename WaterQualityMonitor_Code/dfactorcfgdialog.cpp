#include "dfactorcfgdialog.h"
#include "ui_dfactorcfgdialog.h"
#include "config.h"
#include <QDoubleValidator>
#include <QDebug>

DFactorCfgDialog::DFactorCfgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DFactorCfgDialog)
{
    ui->setupUi(this);
    initUI();
}

DFactorCfgDialog::~DFactorCfgDialog()
{
    qDebug() << "~DFactorCfgDialog()";
    delete ui;
}

void DFactorCfgDialog::initUI()
{
    ui->comboBox_chl->addItem("1");
    ui->comboBox_chl->addItem("2");
    ui->label_chl->setText(tr("Channel"));
    ui->label_factor->setText(tr("Factor"));
    ui->pushButton_cfg->setText(tr("Config"));
    ui->pushButton_read->setText(tr("Read"));

    QDoubleValidator *dVali = new QDoubleValidator(this);
    dVali->setRange(0.0, 9.0, 3);
    dVali->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_factor->setValidator(dVali);

    this->setWindowTitle(tr("Factor Dialog"));

}

void DFactorCfgDialog::on_pushButton_cfg_clicked()
{
    qDebug() << "Config factor";
    switch(ui->comboBox_chl->currentIndex())
    {
    case 0:
        gFactor.firstFactor = ui->lineEdit_factor->text().toFloat();
        break;
    case 1:
        gFactor.secondFactor = ui->lineEdit_factor->text().toFloat();
        break;
    default:
        break;
    }
    MainSaveConfigParam(SaveFactorCfg);
}

void DFactorCfgDialog::on_pushButton_read_clicked()
{
    qDebug() << "Read factor";
    switch(ui->comboBox_chl->currentIndex())
    {
    case 0:
        ui->lineEdit_factor->setText(QString("%1").arg(gFactor.firstFactor));
        break;
    case 1:
        ui->lineEdit_factor->setText(QString("%1").arg(gFactor.secondFactor));
        break;
    default:
        ui->lineEdit_factor->setText("unknow");
        break;
    }

}
