#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "dhexcmd.h"
#include "dlogger.h"

namespace Ui {
class MainWindow;
}

class QSerialPort;
class QLabel;
class QComboBox;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QCheckBox;
class QLineEdit;
class DSqlWorker;
class DSqlDialog;

union DHexFloat
{
    int src;
    float dest;
};

const int TimeInterVal = 1000 * 2; //time interval

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef enum Port_Cfg
    {
        PortName = 0,
        Baud,
        DataBits,
        Parity,
        StopBits,
        FlowControl,
        Cfg_Num,
    }PORT_CFG;

    enum SensorChannel
    {
        Channel_0 = 0,
        Channel_1,
        Channel_2,
        Channel_3,
        Channel_4,
        Channel_Num
    };

    enum UnitString
    {
        UnitRes =0,
        UnitCond,
        UnitTemp,
        Unit_Num
    };

    enum ACTION_NAME
    {
        SQLCONFIG_ACTION = 0,
        FACTORCFG_ACTION,
        SIMTEMP_ACTION,
        ACTION_NUM
    };

    enum MENU_NAME
    {
        CONFIG_MENU = 0,
        MENU_NUM
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void sleep(unsigned int msec);

signals:
    void data(int, const float, const float, const QString&);

protected slots:
    void onConfigbuttonToggled(bool checked);
    void onClearbuttonClicked();
    void onUpdportbuttonClicked();

    void onCheckboxStatechanged(int state);
    void onAllcheckboxStatechanged(int state);

    void onReadconstbtnClicked();
    void onWriteconstbtnClicked();
    void onReadtempconstbtnClicked();
    void onWritetempconstbtnClicked();
    void sendCommand();

    //for sql
    void showSqlDialog();

    void showFactorCfgDialog();
    void showTempSimDialog();

private:
    void readValue();
    void saveConst();
    void writeParameter(DHexCmd::CommandType cmdType);

    void transmit(const QByteArray &cmd, DHexCmd::CommandType CmdType = DHexCmd::ReadValue);

    void init();
    void initUnitString();
    void createWidget(); // 创建子控件
    void initWidget();   // 创建控件文字描述
    void initLayout();   // 创建布局
    void updateSerialPortInfo();
    void initConnect();
    void initMenuBar();

    void configSerialPort();
    void setPortName(const QString& portName);
    void setBaudRate(const QString& rate);
    void setParity(int parity);
    void setDataBits(int databits);
    void setStopBits(int stopbits);
    void setFlowControl(int flow);

    //
    void analysisChannel(QByteArray& bytes, DHexCmd::CommandType CmdType = DHexCmd::ReadValue);
    void analysisData(int ch, QByteArray& bytes, DHexCmd::CommandType CmdType = DHexCmd::ReadValue);
    void analysisReadData(int ch, QByteArray& bytes);
    void analysisReadConst(int ch, QByteArray& bytes, DHexCmd::CommandType CmdType = DHexCmd::ReadValue);

    void setFunEnabled(bool enable);

    //for sql
    void initSqlWorker();

    float reCalcRx(int iChl, float fValue, float tx);
private:
    Ui::MainWindow *ui;
    
    QSerialPort* m_serialPort;
    DHexCmd m_hexCmd;
    QByteArray m_serialData;
    QTimer *m_workTimer;

    QLabel *m_portLabel[Cfg_Num];
    QComboBox *m_portComboBox[Cfg_Num];
    QPushButton *m_configButton;
    QPushButton *m_clearButton;
    QPushButton *m_updPortButton;
    QCheckBox *m_pAllCheckBox;

    QGroupBox *m_configGroup;

    QLabel *m_pChannelLabel;
    QComboBox *m_pChannelComboBox;

    QLabel *m_pConstLabel;
    QLabel *m_pTempConstLabel;

    QLineEdit *m_pConstLineEdit;
    QPushButton *m_pReadConstBtn;
    QPushButton *m_pWriteConstBtn;

    QLineEdit *m_pTempConstLineEdit;
    QPushButton *m_pReadTempConstBtn;
    QPushButton *m_pWriteTempConstBtn;

    QGroupBox *m_caliGroup;

    QLabel *m_statusLabel;

    QCheckBox *m_pChannelCheckBox[Channel_Num];
    QLabel *m_pQualityLabel[Channel_Num];
    QLabel *m_pTempLabel[Channel_Num];
    QLabel *m_pReadCountLabel[Channel_Num];
    int m_readCount[Channel_Num];

    int m_iMask;
    QString m_strUnit[Unit_Num];

    //for menu
    QAction *m_pAction[ACTION_NUM];
    QMenu *m_pMenu[MENU_NUM];

    //for sql
    QThread m_sqlWorkerThread;
    DSqlWorker *m_pSqlWorker;
    DSqlDialog *m_pSqlDlg;
};

#endif // MAINWINDOW_H
