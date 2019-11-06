#include "dcommandgenerator.h"

unsigned short calcrc16(const QByteArray &buf)
{
    int len = buf.length();
    unsigned short crc = 0;
    int i,j;
    for ( j = 0; j < len; j++)
    {
        unsigned char b = buf[j];
        for (i = 0; i < 8; i++)
        {
            crc = ((b ^ (unsigned char)crc) & 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1);
            b >>= 1;
        }
    }
    return crc;
}

DCommandGenerator::DCommandGenerator(const QString &cmd)
{
    stringToHex(cmd);
}

void DCommandGenerator::setCommand(const QString &cmd)
{
    stringToHex(cmd);
}

const QByteArray &DCommandGenerator::hexCommand(bool crc)
{
    if(crc)
    {
        doCalcrc16(m_hexCommand);
    }
    return m_hexCommand;
}

char DCommandGenerator::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    {
        return ch - 0x30;
    }
    else if((ch >= 'a') && (ch <= 'f'))
    {
        return ch - 'a' + 10;
    }
    else if((ch >= 'A') && (ch <= 'F'))
    {
        return ch - 'A' + 10;
    }
    else
    {
        return ch - ch;
    }
}

void DCommandGenerator::stringToHex(const QString &str)
{
    char lstr, hstr;
    int hHexData, lHexData, hexData;
    int dataLen = 0;
    int len = str.length();
    m_hexCommand.resize(len/2);
    for(int i = 0; i < len;)
    {
        hstr = str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
        {
            break;
        }
        lstr = str[i].toLatin1();

        hHexData = convertHexChar(hstr);
        lHexData = convertHexChar(lstr);
        if(lstr == 16 || hHexData == 16)
        {
            break;
        }
        else
        {
            hexData = hHexData*16 + lHexData;
        }
        i++;
        m_hexCommand[dataLen] = (char)hexData;
        dataLen++;
    }
    m_hexCommand.resize(dataLen);
}

void DCommandGenerator::doCalcrc16(QByteArray &buff)
{
     unsigned short usCrc = calcrc16(buff);
     buff.append((usCrc >> 8) & 0xff);
     buff.append((usCrc >> 0) & 0xff);
}
