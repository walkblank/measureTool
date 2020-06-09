#include "md19client.h"

Md19Client::Md19Client(QObject *parent)
    : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

void Md19Client::onDataReady()
{
    QByteArray readdata = readAll();
}
// cmd = 10;
int Md19Client::writeRegN(char cmd, char addrH, char addrL, int regNum, char *writeData, int dataLen)
{
    // 5, startcode,
    int ret;
    int totalLen =  5+1 +1+1 +2+2 + 1+ dataLen;
    char contentLen = 1+1+2+2 + 1 + dataLen;
    char dtLen = dataLen;
    qDebug("contentLen: %d\n", contentLen);
    qDebug("dtLen : %d\n", dtLen);
    char *data = new char[totalLen];
    memset(data, 0, totalLen);
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = contentLen;
    data[6] = 1; // module address
    data[7] = cmd;
    data[8] = addrH;
    data[9] = addrL;
    data[10] = 0;
    data[11] = regNum;
    data[12] = dtLen;
    memcpy(data+13, writeData, dataLen);

    for(int i = 0; i < totalLen; i ++)
    {
        qDebug("data[%d]: %x\n", i, data[i]);
    }

    ret = write(data, totalLen);
    qDebug()<<"write" << ret;
    delete []data;
    return  ret;
}

// cmd = 05
int Md19Client::writeRegS(char cmd, char addrH, char addrL, char writeDataH, char writeDataL)
{
    int ret;

    char data[12];
    data[0] =  0;
    data[1] =  0;
    data[2] =  0;
    data[3] =  0;
    data[4] =  0;
    data[5] =  6;
    data[6] =  1;
    data[7] = cmd;
    data[8] = addrH;
    data[9] = addrL;
    data[10] = writeDataH;
    data[11] = writeDataL;

    ret = write(data, 12);

    return ret;
}

// read 8 di , cmd 2; read 8 AI, cmd 4
int Md19Client::readReg(char cmd, char addrH, char addrL, int regNum)
{
    int ret;
    char data[12];
    data[0] =  0;
    data[1] =  0;
    data[2] =  0;
    data[3] =  0;
    data[4] =  0;
    data[5] =  6;
    data[6] =  1;
    data[7] = cmd;
    data[8] = addrH;
    data[9] = addrL;
    data[10] = 0;
    data[11] = regNum;
    ret = write(data, 12);
    return ret;
}

