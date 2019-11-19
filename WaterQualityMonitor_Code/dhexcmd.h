#ifndef DHEXCMD_H
#define DHEXCMD_H

#include <QObject>
#include <QMap>

class QXmlStreamReader;

class DHexCmd : public QObject
{
    Q_OBJECT

 public:
    enum CommandType
    {
        ReadValue = 0,
        ReadConst,
        ReadTempConst,
        WriteConst,
        WriteTempConst,
        CommandTypeNum
    };
    //函数成员指针
    using XmlFunPtr = void (DHexCmd::*)(QXmlStreamReader &xmlReader);

public:
    explicit DHexCmd(QObject *parent = nullptr);

    const QByteArray command(CommandType type, int iChl) const;
    const QByteArray confirmCommand() const;
    const QByteArray saveCommand() const;

    void displayCommand() const;
private:
    void init();
    bool initFromXml(const QString &fileName);
    void initCommandMap(CommandType type, int iKey, const QByteArray &command);

    void initFunMap();
    void initCmdHeaper(QXmlStreamReader &xmlReader, CommandType cmdType, bool crc = true);
    void initReadCmd(QXmlStreamReader &xmlReader);
    void initReadConstCmd(QXmlStreamReader &xmlReader);
    void initReadTempConstCmd(QXmlStreamReader &xmlReader);
    void initWriteConstCmd(QXmlStreamReader &xmlReader);
    void initWriteTempConstCmd(QXmlStreamReader &xmlReader);
    void initConfirmCmd(QXmlStreamReader &xmlReader);
    void initSaveCmd(QXmlStreamReader &xmlReader);
    void initInvalidKey(QXmlStreamReader &xmlReader);

private:
    QMap<QString, XmlFunPtr> m_funcMap;

    QMap<int, QByteArray> m_commandMap[CommandTypeNum];

    QByteArray m_confirmCmd;
    QByteArray m_saveCmd;

};

#endif // DHEXCMD_H
