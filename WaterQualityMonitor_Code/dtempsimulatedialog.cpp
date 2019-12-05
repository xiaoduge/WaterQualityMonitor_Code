#include "dtempsimulatedialog.h"
#include "ui_dtempsimulatedialog.h"
#include "config.h"
#include <QIntValidator>

DTempSimulateDialog::DTempSimulateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DTempSimulateDialog)
{
    ui->setupUi(this);
    initUI();
}

DTempSimulateDialog::~DTempSimulateDialog()
{
    delete ui;
}

void DTempSimulateDialog::initUI()
{
    ui->checkBox_sw->setText(tr("Enable Simulation"));
    ui->label_temp->setText(tr("Temperature value"));
    ui->pushButton_confirm->setText(tr("Confirm"));

    ui->checkBox_sw->setChecked(gConfig.enableTempSlm);
    ui->lineEdit_temp->setWhatsThis(tr("Enter an integer value obtained by amplifying a temperature value by 10 times"));

    QIntValidator * iVali = new QIntValidator(0, 1000, this);
    ui->lineEdit_temp->setValidator(iVali);

    this->setWindowTitle(tr("Temp Simulation"));
}

void DTempSimulateDialog::on_checkBox_sw_stateChanged(int arg1)
{
    switch (arg1)
    {
    case Qt::Checked:
        gConfig.enableTempSlm = true;
        break;
    case Qt::Unchecked:
        gConfig.enableTempSlm = false;
        break;
    default:
        break;
    }
    MainSaveConfigParam(SaveTempSimCfg);
}

void DTempSimulateDialog::on_pushButton_confirm_clicked()
{
    gSimTemp = ui->lineEdit_temp->text().toInt()/10;
}
