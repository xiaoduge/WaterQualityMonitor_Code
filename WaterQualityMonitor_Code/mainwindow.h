#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dhexcmd.h"

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

union DHexFloat
{
    int src;
    float dest;
};

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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void sleep(unsigned int msec);

protected slots:
    void on_configButton_toggled(bool checked);
    void on_clearButton_clicked();
    void on_updPortButton_clicked();

    void on_checkBox_stateChanged(int state);
    void on_allCheckBox_stateChanged(int state);

    void on_readConstBtn_clicked();
    void on_writeConstBtn_clicked();
    void on_readTempConstBtn_clicked();
    void on_writeTempConstBtn_clicked();
    void sendCommand();

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
};

#endif // MAINWINDOW_H
