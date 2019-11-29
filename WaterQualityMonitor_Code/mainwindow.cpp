#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dcommandgenerator.h"
#include "dhintdialog.h"
#include "dwarningdialog.h"
#include "dsqldialog.h"
#include "dsqlworker.h"
#include <QDebug>
#include "dhexcmd.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <QCheckBox>
#include <QLineEdit>
#include "dlogger.h"
#include "config.h"
#include <QSettings>
#include <QAction>

DLogger gLogger("Log");
GlobalConfiguration gConfig;

/**
 * 读取配置文件，在构造MainWindow之前调用
 */
void MainRetriveConfigParam()
{
    QSettings *config = new QSettings(QStringLiteral("config.ini"), QSettings::IniFormat);

    QString strV = "/OpenSqlDataLogger/";
    gConfig.openDataLogger = config->value(strV, false).toBool();

    if (config)
    {
        config->deleteLater();
        config = Q_NULLPTR;
    }
    gLogger.log("Retrive config param", __FILE__, __LINE__);
}

/**
 * 保存配置信息
 */
void MainSaveConfigParam()
{
    QSettings *config = new QSettings(QStringLiteral("config.ini"), QSettings::IniFormat);

    QString strV = "/OpenSqlDataLogger/";
    config->setValue(strV, gConfig.openDataLogger);

    if (config)
    {
        config->deleteLater();
        config = Q_NULLPTR;
    }
    gLogger.log("Save config param", __FILE__, __LINE__);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_readCount{0, 0, 0, 0, 0},
    m_iMask(0)
{
    ui->setupUi(this);
    init();
//    m_hexCmd.displayCommand();
}

MainWindow::~MainWindow()
{
    if(m_sqlWorkerThread.isRunning())
    {
        m_sqlWorkerThread.quit();
        m_sqlWorkerThread.wait();
        gLogger.log("Sql work thread quit!", __FILE__, __LINE__);
    }

    delete ui;
    gLogger.log("Destroy MainWindow", __FILE__, __LINE__);
}

void MainWindow::sleep(unsigned int msec)
{
    QTime delayTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < delayTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void MainWindow::onConfigbuttonToggled(bool checked)
{
    if(checked)
    {
        configSerialPort();
        if(!m_serialPort->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this, tr("Warning"), tr("Open Serial Port Failed!"));
            gLogger.log("Open serial port failed", __FILE__, __LINE__, DLogger::Log_Error);
            m_configButton->setChecked(false);
            return;
        }
        m_configButton->setText(tr("Close"));
        m_statusLabel->setText("<font face='Arial' size='5' color='green'>" + tr("Open") + "</font>");
        m_workTimer->start(TimeInterVal);
        setFunEnabled(true);
        gLogger.log("Open Serial Port", __FILE__, __LINE__);

    }
    else
    {
        m_configButton->setText(tr("Open"));

        if(m_serialPort->isOpen())
        {
            m_serialPort->close();
        }

        m_statusLabel->setText("<font face='Arial' size='5' color='white'>" + tr("Close") + "</font>");
        m_workTimer->stop();
        setFunEnabled(false);
        gLogger.log("Close Serial Port", __FILE__, __LINE__);
    }
}

void MainWindow::onClearbuttonClicked()
{
    memset(m_readCount, 0, sizeof(m_readCount));
    for(int i = 0; i < Channel_Num; ++i)
    {
        m_pReadCountLabel[i]->setText(QString("Rx: %1").arg(m_readCount[i]));
    }
    m_pConstLineEdit->clear();
    m_pTempConstLineEdit->clear();
}

void MainWindow::onUpdportbuttonClicked()
{
    if(m_serialPort->isOpen())
    {
        return;
    }
    updateSerialPortInfo();
}

void MainWindow::onCheckboxStatechanged(int state)
{
    Q_UNUSED(state);
    QCheckBox *checkBox = qobject_cast<QCheckBox*>(this->sender());
    QString objectName = checkBox->objectName();
    objectName.remove("Channel ");
    int iChl = objectName.toInt();

    if(Qt::Checked == checkBox->checkState())
    {
        m_iMask |= 1 << iChl;
    }
    else
    {
        m_iMask &= ~(1 << iChl);
    }
}

void MainWindow::onAllcheckboxStatechanged(int state)
{
    Q_UNUSED(state);
    if(Qt::Checked == m_pAllCheckBox->checkState())
    {
        for(int i = 0; i < Channel_Num; ++i)
        {
            m_iMask |= 1 << i;
            m_pChannelCheckBox[i]->setCheckState(Qt::Checked);
        }

    }
    else
    {
        for(int i = 0; i < Channel_Num; ++i)
        {
            m_pChannelCheckBox[i]->setCheckState(Qt::Unchecked);
        }
    }
}

void MainWindow::onReadconstbtnClicked()
{
    if(0 != m_iMask)
    {
        DHintDialog::getInstance(tr("Reading data, can't read constant"));
        return;
    }
    int iChl = m_pChannelComboBox->currentIndex();
    QByteArray command = m_hexCmd.command(DHexCmd::ReadConst, iChl);
    transmit(command, DHexCmd::ReadConst);
    gLogger.log("Read electrode constant", __FILE__, __LINE__);
}

void MainWindow::onWriteconstbtnClicked()
{
    writeParameter(DHexCmd::WriteConst);
}

void MainWindow::onReadtempconstbtnClicked()
{
    if(0 != m_iMask)
    {
        DHintDialog::getInstance(tr("Reading data, can't read constant"));
        return;
    }
    int iChl = m_pChannelComboBox->currentIndex();
    QByteArray command = m_hexCmd.command(DHexCmd::ReadTempConst, iChl);
    transmit(command, DHexCmd::ReadTempConst);

    gLogger.log("Read temperature compensation coefficient", __FILE__, __LINE__);
}

void MainWindow::onWritetempconstbtnClicked()
{
     writeParameter(DHexCmd::WriteTempConst);
}

void MainWindow::sendCommand()
{
    if(!m_serialPort->isOpen())
    {
        return;
    }
    readValue();
}

void MainWindow::showSqlDialog()
{
    m_pSqlDlg->show();
}

void MainWindow::readValue()
{
    if(m_iMask & (1 << Channel_0))
    {
        QByteArray command = m_hexCmd.command(DHexCmd::ReadValue, Channel_0);
        transmit(command);
    }

    if(m_iMask & (1 << Channel_1))
    {
        QByteArray command = m_hexCmd.command(DHexCmd::ReadValue, Channel_1);
        transmit(command);
    }

    if(m_iMask & (1 << Channel_2))
    {
        QByteArray command = m_hexCmd.command(DHexCmd::ReadValue, Channel_2);
        transmit(command);
    }
    if(m_iMask & (1 << Channel_3))
    {
        QByteArray command = m_hexCmd.command(DHexCmd::ReadValue, Channel_3);
        transmit(command);
    }
    if(m_iMask & (1 << Channel_4))
    {
        QByteArray command = m_hexCmd.command(DHexCmd::ReadValue, Channel_4);
        transmit(command);
    }
}

void MainWindow::saveConst()
{
    QByteArray saveCommand = m_hexCmd.saveCommand();
    m_serialPort->write(saveCommand);
    sleep(100);
}

void MainWindow::writeParameter(DHexCmd::CommandType cmdType)
{
    if(0 != m_iMask)
    {
        DHintDialog::getInstance(tr("Reading data, can't modify parameters"));
        return;
    }

    DWarningDialog warningDlg(tr("Are you sure you want to modify this parameter?"));
    if(QDialog::Accepted != warningDlg.exec())
    {
        return;
    }

    int iChl = m_pChannelComboBox->currentIndex();
    QByteArray command = m_hexCmd.command(cmdType, iChl);
    qint16 iConst = 0;
    switch (cmdType)
    {
    case DHexCmd::WriteConst:
        iConst = m_pConstLineEdit->text().toUShort();
        if(0 == iConst)
        {
            DHintDialog::getInstance(tr("The electrode constant cannot be zero"));
            return;
        }
        break;
    case DHexCmd::WriteTempConst:
        iConst = m_pTempConstLineEdit->text().toShort();
        break;
    default:
        break;
    }

    command.append(iConst >> 8);
    command.append(iConst);
    DCommandGenerator::doCalcrc16(command);

    m_serialPort->write(command);
    sleep(100);

    saveConst();

    DHintDialog::getInstance(tr("Successfully written parameters"));

    switch (cmdType)
    {
    case DHexCmd::WriteConst:
        gLogger.log("Modified electrode constant", __FILE__, __LINE__);
        break;
    case DHexCmd::WriteTempConst:
        gLogger.log("Modified temperature compensation coefficient", __FILE__, __LINE__);
        break;
    default:
        break;
    }

}

void MainWindow::transmit(const QByteArray &cmd, DHexCmd::CommandType CmdType)
{
    m_serialPort->write(cmd);

    sleep(300);

    QByteArray readBuf = m_serialPort->readAll();

    analysisChannel(readBuf, CmdType);
}

void MainWindow::init()
{
    this->setWindowTitle(tr("WaterQualityMonitor"));
    m_workTimer = new QTimer(this);
    connect(m_workTimer, &QTimer::timeout, this, &MainWindow::sendCommand);

    m_serialPort = new QSerialPort(this);
    m_pSqlDlg = new DSqlDialog(this);
    m_pSqlDlg->setObjectName("SqlDataDialog");

    initUnitString();
    createWidget();
    updateSerialPortInfo();

    initSqlWorker();
    initMenuBar();
    gLogger.log("Construct the MainWindow instance", __FILE__, __LINE__);
}

void MainWindow::initUnitString()
{
    m_strUnit[UnitRes] = tr("MO");
    m_strUnit[UnitCond] = tr("US");
    m_strUnit[UnitTemp] = tr("CC");
}

void MainWindow::createWidget()
{
    for(int i = 0; i < Cfg_Num; i++)
    {
        m_portLabel[i] = new QLabel;
        m_portComboBox[i] = new QComboBox;
    }
    m_configButton = new QPushButton;
    m_configButton->setCheckable(true);
    m_clearButton = new QPushButton;
    m_updPortButton = new QPushButton;
    m_pAllCheckBox = new QCheckBox;

    m_configGroup = new QGroupBox;
    m_caliGroup = new QGroupBox;

    m_pChannelLabel  = new QLabel;
    m_pChannelComboBox = new QComboBox;

    m_pConstLabel = new QLabel;
    m_pTempConstLabel = new QLabel;

    QValidator *validator = new QIntValidator(0, 65535, this);
    m_pConstLineEdit = new QLineEdit;
    m_pConstLineEdit->setValidator(validator);

    QValidator *tempValidator = new QIntValidator(-32758, 32767, this);
    m_pTempConstLineEdit = new QLineEdit;
    m_pTempConstLineEdit->setValidator(tempValidator);

    m_pReadConstBtn = new QPushButton;
    m_pWriteConstBtn = new QPushButton;
    m_pReadTempConstBtn = new QPushButton;
    m_pWriteTempConstBtn = new QPushButton;

    for(int i = 0; i < Channel_Num; ++i)
    {
        m_pChannelCheckBox[i] = new QCheckBox;
        m_pQualityLabel[i] = new QLabel;
        m_pQualityLabel[i]->setAlignment(Qt::AlignCenter);
        m_pTempLabel[i] = new QLabel;
        m_pTempLabel[i]->setAlignment(Qt::AlignCenter);
        m_pReadCountLabel[i] = new QLabel;
        m_pReadCountLabel[i]->setAlignment(Qt::AlignCenter);
    }

    m_statusLabel = new QLabel;
    ui->statusBar->addWidget(m_statusLabel);

    initWidget();
    initLayout(); //启用布局
    initConnect(); //信号与槽
    setFunEnabled(false);
}

void MainWindow::initWidget()
{
    m_portLabel[PortName]->setText(tr("PortName"));
    m_portLabel[Baud]->setText(tr("BaudRate"));
    m_portLabel[DataBits]->setText(tr("DataBits"));
    m_portLabel[Parity]->setText(tr("Parity"));
    m_portLabel[StopBits]->setText(tr("StopBits"));
    m_portLabel[FlowControl]->setText(tr("FlowControl"));

    QStringList baudList;
    baudList << QStringLiteral("1200") << QStringLiteral("2400") << QStringLiteral("4800") << QStringLiteral("9600")
             << QStringLiteral("19200") << QStringLiteral("38400") <<QStringLiteral("57600") << QStringLiteral("115200");
    m_portComboBox[Baud]->addItems(baudList);
    m_portComboBox[Baud]->setCurrentIndex(3);

    QStringList dataList;
    dataList << QStringLiteral("5") << QStringLiteral("6") << QStringLiteral("7") << QStringLiteral("8");
    m_portComboBox[DataBits]->addItems(dataList);
    m_portComboBox[DataBits]->setCurrentIndex(3);


    QStringList parityList;
    parityList << QStringLiteral("None") << QStringLiteral("Even") << QStringLiteral("Odd")
               << QStringLiteral("Space") << QStringLiteral("Mark");
    m_portComboBox[Parity]->addItems(parityList);
    m_portComboBox[Parity]->setCurrentIndex(0);

    QStringList stopList;
    stopList << QStringLiteral("1") << QStringLiteral("1.5") << QStringLiteral("2");
    m_portComboBox[StopBits]->addItems(stopList);
    m_portComboBox[StopBits]->setCurrentIndex(0);

    QStringList flowList;
    flowList << QStringLiteral("None") << QStringLiteral("RTS/CTS") << QStringLiteral("XON/XOFF");
    m_portComboBox[FlowControl]->addItems(flowList);
    m_portComboBox[FlowControl]->setCurrentIndex(0);

    m_configButton->setText(tr("Open"));
    m_clearButton->setText(tr("Clear"));
    m_updPortButton->setText(tr("Refresh"));
    m_pAllCheckBox->setText(tr("Read All"));

    m_pChannelLabel->setText(tr("Channel"));

    m_pConstLabel->setText(tr("Const"));
    m_pTempConstLabel->setText(tr("Temp Const"));

    m_pReadConstBtn->setText(tr("Read"));
    m_pWriteConstBtn->setText(tr("Write"));

    m_pReadTempConstBtn->setText(tr("Read"));
    m_pWriteTempConstBtn->setText(tr("Write"));

    for(int i = 0; i < Channel_Num; ++i)
    {
        m_pChannelComboBox->addItem(tr("Channel") + QString(" %1").arg(i));

        m_pChannelCheckBox[i]->setText(tr("Channel") + QString(" %1").arg(i));
        m_pChannelCheckBox[i]->setObjectName(QString("Channel %1").arg(i));
        if(i < 2)
        {
            m_pQualityLabel[i]->setText(QString("%1 ").arg(0) + m_strUnit[UnitCond]);
        }
        else
        {
            m_pQualityLabel[i]->setText(QString("%1 ").arg(0) + m_strUnit[UnitRes]);
        }
        m_pTempLabel[i]->setText(QString("%1 ").arg(0) + m_strUnit[UnitTemp]);
        m_pReadCountLabel[i]->setText(QStringLiteral("Rx: 0"));
    }


    m_statusLabel->setText("<font face='Arial' size='5' color='white'>" + tr("Close") + "</font>");
    QString appInfo = "<font face='Consolas' size='4' color='white'>" + qApp->applicationVersion() + "</font>";
    ui->statusBar->addPermanentWidget(new QLabel(appInfo, this));
}

void MainWindow::initLayout()
{
    QFormLayout* cfgLayout = new QFormLayout;
    for(int i = 0; i < Cfg_Num; i++)
    {
        cfgLayout->addRow(m_portLabel[i], m_portComboBox[i]);
    }
    cfgLayout->addRow(m_configButton);
    cfgLayout->addRow(m_clearButton);
    cfgLayout->addRow(m_updPortButton);
    m_configGroup->setLayout(cfgLayout);

    QFormLayout* cailLayout = new QFormLayout;
    cailLayout->addRow(m_pChannelLabel, m_pChannelComboBox);
    cailLayout->addRow(new QLabel(""));
    cailLayout->addRow(m_pConstLabel, m_pConstLineEdit);
    cailLayout->addRow(m_pReadConstBtn);
    cailLayout->addRow(m_pWriteConstBtn);
    cailLayout->addRow(new QLabel(""));
    cailLayout->addRow(m_pTempConstLabel, m_pTempConstLineEdit);
    cailLayout->addRow(m_pReadTempConstBtn);
    cailLayout->addRow(m_pWriteTempConstBtn);
    m_caliGroup->setLayout(cailLayout);

    QSpacerItem* spacerItem = new QSpacerItem(2, 10);
    QVBoxLayout* leftVLayout = new QVBoxLayout;
    leftVLayout->addWidget(m_configGroup);
    leftVLayout->addSpacerItem(spacerItem);
    leftVLayout->addWidget(m_caliGroup);

    QHBoxLayout *valueLayout = new QHBoxLayout;

    for(int i = 0; i < Channel_Num; ++i)
    {
        QVBoxLayout *chlLayout = new QVBoxLayout;
        chlLayout->setAlignment(Qt::AlignCenter);
        chlLayout->addStretch(30);
        chlLayout->addWidget(m_pQualityLabel[i]);
        chlLayout->addStretch(10);
        chlLayout->addWidget(m_pTempLabel[i]);
        chlLayout->addStretch(20);
        chlLayout->addWidget(m_pChannelCheckBox[i]);
        chlLayout->addStretch(20);
        chlLayout->addWidget(m_pReadCountLabel[i]);
        chlLayout->addStretch(30);
        QFrame *tempFrame = new QFrame;
        tempFrame->setObjectName("tempFrame");
        tempFrame->setLayout(chlLayout);
        valueLayout->addWidget(tempFrame);
    }
    QVBoxLayout *frameLayout = new QVBoxLayout;
    QHBoxLayout *checkLayout = new QHBoxLayout;
    checkLayout->setAlignment(Qt::AlignRight);
    checkLayout->addWidget(m_pAllCheckBox);
    frameLayout->addLayout(valueLayout);
    frameLayout->addLayout(checkLayout);
    QFrame *valueFrame = new QFrame;
    valueFrame->setLayout(frameLayout);
    valueFrame->setLineWidth(2);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addLayout(leftVLayout);
    topLayout->addSpacing(20);
    topLayout->addWidget(valueFrame);

    topLayout->setStretchFactor(leftVLayout, 2);
    topLayout->setStretchFactor(valueFrame, 8);

    ui->centralWidget->setLayout(topLayout);

}

void MainWindow::updateSerialPortInfo()
{
    m_portComboBox[PortName]->clear();
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serialPort;
        serialPort.setPort(info);
        if(serialPort.open(QIODevice::ReadWrite))
        {
            m_portComboBox[PortName]->addItem(serialPort.portName());
            serialPort.close();
        }
    }
}

void MainWindow::initConnect()
{
    connect(m_configButton, SIGNAL(toggled(bool)), this, SLOT(onConfigbuttonToggled(bool)));
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearbuttonClicked);
    connect(m_updPortButton, &QPushButton::clicked, this, &MainWindow::onUpdportbuttonClicked);

    connect(m_pReadConstBtn, &QPushButton::clicked, this, &MainWindow::onReadconstbtnClicked);
    connect(m_pWriteConstBtn, &QPushButton::clicked, this, &MainWindow::onWriteconstbtnClicked);

    connect(m_pReadTempConstBtn, &QPushButton::clicked, this, &MainWindow::onReadtempconstbtnClicked);
    connect(m_pWriteTempConstBtn, &QPushButton::clicked, this, &MainWindow::onWritetempconstbtnClicked);

    for(int i = 0; i < Channel_Num; ++i)
    {
        connect(m_pChannelCheckBox[i], &QCheckBox::stateChanged, this, &MainWindow::onCheckboxStatechanged);
    }

    connect(m_pAllCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onAllcheckboxStatechanged);

}

void MainWindow::initMenuBar()
{
    m_pMenu[CONFIG_MENU] = ui->menuBar->addMenu(tr("Config"));
    m_pAction[SQLCONFIG_ACTION] = new QAction(tr("Sql Config"), this);
    m_pMenu[CONFIG_MENU]->addAction(m_pAction[SQLCONFIG_ACTION]);
    connect(m_pAction[SQLCONFIG_ACTION], SIGNAL(triggered()), this, SLOT(showSqlDialog()));
}

void MainWindow::configSerialPort()
{
    setPortName(m_portComboBox[PortName]->currentText());
    setBaudRate(m_portComboBox[Baud]->currentText());
    setDataBits(m_portComboBox[DataBits]->currentIndex());
    setParity(m_portComboBox[Parity]->currentIndex());
    setStopBits(m_portComboBox[StopBits]->currentIndex());
    setFlowControl(m_portComboBox[FlowControl]->currentIndex());
}

void MainWindow::setPortName(const QString &portName)
{
    m_serialPort->setPortName(portName);
}

void MainWindow::setBaudRate(const QString &rate)
{
    m_serialPort->setBaudRate(rate.toInt());
}

void MainWindow::setParity(int parity)
{
    switch (parity)
    {
    case 0:
        m_serialPort->setParity(QSerialPort::NoParity);
        break;
    case 1:
        m_serialPort->setParity(QSerialPort::EvenParity);
        break;
    case 2:
        m_serialPort->setParity(QSerialPort::OddParity);
        break;
    case 3:
        m_serialPort->setParity(QSerialPort::SpaceParity);
        break;
    case 4:
        m_serialPort->setParity(QSerialPort::MarkParity);
        break;
    default:
        break;
    }
}

void MainWindow::setDataBits(int databits)
{
    switch(databits)
    {
    case 0:
        m_serialPort->setDataBits(QSerialPort::Data5);
        break;
    case 1:
        m_serialPort->setDataBits(QSerialPort::Data6);
        break;
    case 2:
        m_serialPort->setDataBits(QSerialPort::Data7);
        break;
    case 3:
        m_serialPort->setDataBits(QSerialPort::Data8);
        break;
    default:
        break;
    }
}

void MainWindow::setStopBits(int stopbits)
{
    switch(stopbits)
    {
    case 0:
        m_serialPort->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        m_serialPort->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        m_serialPort->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        break;
    }
}

void MainWindow::setFlowControl(int flow)
{
    switch(flow)
    {
    case 0:
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        break;
    case 1:
        m_serialPort->setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        m_serialPort->setFlowControl(QSerialPort::SoftwareControl);
        break;
    default:
        break;
    }
}

void MainWindow::analysisChannel(QByteArray &bytes, DHexCmd::CommandType CmdType)
{
    int ch = (bytes[0]&0xFF)*256 + (bytes[1]&0xFF);
    switch (ch)
    {
    case 0xc102:
        analysisData(0, bytes, CmdType);
        break;
    case 0xc202:
        analysisData(1, bytes, CmdType);
        break;
    case 0xc302:
        analysisData(2, bytes, CmdType);
        break;
    case 0xc402:
        analysisData(3, bytes, CmdType);
        break;
    case 0xc502:
        analysisData(4, bytes, CmdType);
        break;
    default:
        m_serialPort->clear();
        gLogger.log("analysisChannel: error", __FILE__, __LINE__);
        break;
    }
}

void MainWindow::analysisData(int ch, QByteArray &bytes, DHexCmd::CommandType CmdType)
{
    int dataBits = bytes[2]&0xFF;
    switch (dataBits)
    {
    case 2:
        analysisReadConst(ch, bytes, CmdType);
        break;
    case 6:
        analysisReadData(ch, bytes);
        break;
    default:
        m_serialPort->clear();
        gLogger.log("analysisData: error", __FILE__, __LINE__);
        break;
    }

}

void MainWindow::analysisReadData(int ch, QByteArray &bytes)
{
    QByteArray byteArray;
    byteArray = bytes;
    QByteArray rBuf = byteArray.mid(3, 4);
    QString strR;
    QDataStream out1(&rBuf, QIODevice::ReadWrite);
    while(!out1.atEnd())
    {
        qint8 outChar = 0;
        out1 >> outChar;

        QString strReceive = QString("%1").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
        strR += strReceive;
    }

    QByteArray tBuf = byteArray.mid(7, 2);
    QString strT;
    QDataStream out2(&tBuf, QIODevice::ReadWrite);
    while(!out2.atEnd())
    {
        qint8 outChar = 0;
        out2 >> outChar;

        QString strReceive = QString("%1").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
        strT += strReceive;
    }

    DHexFloat hexFloat;
    bool ok;
    hexFloat.src = strR.toLong(&ok, 16)&0xFFFFFFFF;
    if(!ok)
    {
        gLogger.log("String to long error", __FILE__, __LINE__, DLogger::Log_Error);
        return;
    }

    if(ch < 2)
    {
        if(hexFloat.dest < 1.0)
        {
            m_pQualityLabel[ch]->setText(QString("%1 ").arg(hexFloat.dest, 0, 'f', 3) + m_strUnit[UnitCond]);
        }
        else
        {
            m_pQualityLabel[ch]->setText(QString("%1 ").arg(hexFloat.dest, 0, 'f', 1) + m_strUnit[UnitCond]);
        }

    }
    else
    {
        if(hexFloat.dest < 1.0)
        {
            m_pQualityLabel[ch]->setText(QString("%1 ").arg(hexFloat.dest, 0, 'f', 3) + m_strUnit[UnitRes]);
        }
        else
        {
            m_pQualityLabel[ch]->setText(QString("%1 ").arg(hexFloat.dest, 0, 'f', 1) + m_strUnit[UnitRes]);
        }
    }
    m_pTempLabel[ch]->setText(QString("%1 ").arg(strT.toInt(&ok, 16)/10.0) + m_strUnit[UnitTemp]);
    ++m_readCount[ch];
    m_pReadCountLabel[ch]->setText(QString("Rx: %1").arg(m_readCount[ch]));
    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //for sql
    if(gConfig.openDataLogger)
    {
        emit data(ch, hexFloat.dest, strT.toInt(&ok, 16)/10.0, curTime);
    }

}

void MainWindow::analysisReadConst(int ch, QByteArray &bytes, DHexCmd::CommandType CmdType)
{
    Q_UNUSED(ch);
    qint16 constValue = (bytes[3]&0xFF)*256 + (bytes[4]&0xFF);
    switch (CmdType)
    {
    case DHexCmd::ReadConst:
        m_pConstLineEdit->setText(QString("%1").arg(constValue));
        break;
    case DHexCmd::ReadTempConst:
        m_pTempConstLineEdit->setText(QString("%1").arg(constValue));
        break;
    default:
        break;
    }
}

void MainWindow::setFunEnabled(bool enable)
{
    for(int i = 0; i < Channel_Num; ++i)
    {
        m_pChannelCheckBox[i]->setEnabled(enable);
    }
    m_pReadConstBtn->setEnabled(enable);
    m_pWriteConstBtn->setEnabled(enable);
    m_pReadTempConstBtn->setEnabled(enable);
    m_pWriteTempConstBtn->setEnabled(enable);
    m_pAllCheckBox->setEnabled(enable);
}

void MainWindow::initSqlWorker()
{
    m_pSqlWorker = new DSqlWorker;
    m_pSqlWorker->moveToThread(&m_sqlWorkerThread);

    connect(&m_sqlWorkerThread, SIGNAL(finished()), m_pSqlWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(data(int, const float, const float, const QString&)),
            m_pSqlWorker, SLOT(updateSqlData(int, const float, const float, const QString&)));
    m_sqlWorkerThread.start();
}
