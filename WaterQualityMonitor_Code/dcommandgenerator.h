#ifndef DCOMMANDGENERATOR_H
#define DCOMMANDGENERATOR_H

#include <QString>

class DCommandGenerator
{
public:
    DCommandGenerator() = default;
    explicit DCommandGenerator(const QString &cmd);

    void setCommand(const QString &cmd);
    const QByteArray &hexCommand(bool crc = true);

    static void doCalcrc16(QByteArray &buff);

private:
    char convertHexChar(char ch);
    void stringToHex(const QString &str);
//    void doCalcrc16();

private:
    QByteArray m_hexCommand;
};

unsigned short calcrc16(const QByteArray &buf);

#endif // DCOMMANDGENERATOR_H
