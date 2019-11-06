#include "dhexcmd.h"
#include "dcommandgenerator.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDebug>

DHexCmd::DHexCmd(QObject *parent) : QObject(parent)
{
    init();
}

const QByteArray DHexCmd::command(DHexCmd::CommandType type, int iChl) const
{
    return m_commandMap[type].value(iChl);
}

const QByteArray DHexCmd::confirmCommand() const
{
    return m_confirmCmd;
}

const QByteArray DHexCmd::saveCommand() const
{
    return m_saveCmd;
}

const void DHexCmd::displayCommand() const
{
    for(int i = 0; i < CommandTypeNum; i++)
    {
        auto iter = m_commandMap[i].constBegin();
        for(; iter != m_commandMap[i].constEnd(); ++iter)
        {
            qDebug() << iter.key() << " : " << iter.value();
        }
    }
}

void DHexCmd::init()
{
    initFunMap();

    if(!initFromXml(":/xml/command.xml"))
    {
        QMessageBox::critical(nullptr,
                              QStringLiteral("Error"),
                              QStringLiteral("Open XML File failed!"));
    }

}

bool DHexCmd::initFromXml(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QXmlStreamReader xmlReader(&file);

    while(!xmlReader.atEnd())
    {
        QXmlStreamReader::TokenType type = xmlReader.readNext();
        if(type == QXmlStreamReader::StartElement)
        {
            XmlFunPtr funcPtr = m_funcMap.value(xmlReader.name().toString(), &DHexCmd::initInvalidKey);
            (this->*funcPtr)(xmlReader);
        }
    }

    file.close();
    return true;
}

void DHexCmd::initCommandMap(DHexCmd::CommandType type, int iKey, const QByteArray &command)
{
    m_commandMap[type].insert(iKey, command);
}

void DHexCmd::initFunMap()
{
    m_funcMap.insert("ReadData", &DHexCmd::initReadCmd);
    m_funcMap.insert("ReadConst", &DHexCmd::initReadConstCmd);
    m_funcMap.insert("ReadTempConst", &DHexCmd::initReadTempConstCmd);
    m_funcMap.insert("WriteConst", &DHexCmd::initWriteConstCmd);
    m_funcMap.insert("WriteTempConst", &DHexCmd::initWriteTempConstCmd);
    m_funcMap.insert("Confirm", &DHexCmd::initConfirmCmd);
    m_funcMap.insert("Save", &DHexCmd::initSaveCmd);
}

void DHexCmd::initCmdHeaper(QXmlStreamReader &xmlReader, DHexCmd::CommandType cmdType, bool crc)
{
    if(xmlReader.attributes().hasAttribute("id"))
    {
        int iChl = xmlReader.attributes().value("id").toInt();
        QString strCmd = xmlReader.readElementText();
        DCommandGenerator cmd(strCmd);
        QByteArray bytes = cmd.hexCommand(crc);
        initCommandMap(cmdType, iChl, bytes);
    }
}

void DHexCmd::initReadCmd(QXmlStreamReader &xmlReader)
{
    initCmdHeaper(xmlReader, ReadValue);
}

void DHexCmd::initReadConstCmd(QXmlStreamReader &xmlReader)
{
    initCmdHeaper(xmlReader, ReadConst);
}

void DHexCmd::initReadTempConstCmd(QXmlStreamReader &xmlReader)
{
    initCmdHeaper(xmlReader, ReadTempConst);
}

void DHexCmd::initWriteConstCmd(QXmlStreamReader &xmlReader)
{
    initCmdHeaper(xmlReader, WriteConst, false);
}

void DHexCmd::initWriteTempConstCmd(QXmlStreamReader &xmlReader)
{
    initCmdHeaper(xmlReader, WriteTempConst, false);
}

void DHexCmd::initConfirmCmd(QXmlStreamReader &xmlReader)
{
    QString strCmd = xmlReader.readElementText();
    DCommandGenerator cmd(strCmd);
    m_confirmCmd = cmd.hexCommand();

}

void DHexCmd::initSaveCmd(QXmlStreamReader &xmlReader)
{
    QString strCmd = xmlReader.readElementText();
    DCommandGenerator cmd(strCmd);
    m_saveCmd = cmd.hexCommand();
}

void DHexCmd::initInvalidKey(QXmlStreamReader &xmlReader)
{
    Q_UNUSED(xmlReader);
}

