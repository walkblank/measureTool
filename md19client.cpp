#include "md19client.h"

Md19Client::Md19Client(QObject *parent)
    : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

int  Md19Client::writeMyData(const char *data, int writeLen)
{
    QByteArray sendData = QByteArray(data, writeLen);
    qDebug()<< "sendData" << QString(sendData.toHex(' '));
    return write(data, writeLen);
}

void Md19Client::onDataReady()
{
    //    static QByteArray leftData = QByteArray();
    //    static int step = 0; // 0,start; 1, content ;2, fin;
    QByteArray readData = readAll();
    //    readData.prepend(leftData);
    //    if(step == 0)
    //    {

    //    }
    qDebug()<<"on data Recv" << readData.size();

    qDebug()<< QString(readData.toHex(' '));

    int readLen = readData.size();
    if(readLen < 6)
        return;

    for(int i = 0; i < 5; i ++)
    {
        if(readData.data()[i] != 0)
            return;
    }

    int dataLen = readData.data()[5];
    if(readLen < 6+ dataLen)
        return;

    qDebug()<<"dataLen" << dataLen;
    char cmd = readData.data()[7];
    qDebug()<< "cmd" << cmd;
    switch(cmd)
    {

    case 0x5: // start and stop
    {
        char val = readData.data()[10];
        val == 0 ? beStart = false : beStart = true;
        emit sigData(0x5, beStart, 0);
    }
        break;

    case 0x4: // read temp and flowrate
        emit sigData(0x4,
                     QString(QByteArray(readData.data()+8, 2).toHex()),
                     QString(QByteArray(readData.data()+10,2).toHex()));
        break;
    case 0x10:
        emit sigData(0x10, 0, 0);
        break;
    }
}

void Md19Client::startDev()
{
    writeRegS(0x5, 0, 0, 0xff, 0);
}

void Md19Client::stopDev()
{
    writeRegS(0x5, 0, 0, 0x00, 0);
}

// 4, 6, 7
void Md19Client::setTemp(char temp, char xishiV)
{
    char data[4];
    data[0] = 0;
    data[1] = temp;
    data[2] = 0;
    data[3] = xishiV;
    writeRegN(0x10, 0x00, 0x00, 0x2, data, 4);
}

void Md19Client::readTemp()
{
    readReg(4, 0x0, 0x0, 2);
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



//    QByteArray dataSend = QByteArray(data, totalLen);
//    qDebug()<< QString(dataSend.toHex(' '));

    ret = writeMyData(data, totalLen);
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

    ret = writeMyData(data, 12);

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
    ret = writeMyData(data, 12);

    return ret;
}

