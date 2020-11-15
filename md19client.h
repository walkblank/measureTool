#ifndef MD19CLIENT_H
#define MD19CLIENT_H

#include <QTcpSocket>
#include <QMap>

typedef struct {
    char startCode[5];
    char dataLen;
} MdDataHeader;


class Md19Client : public QTcpSocket
{
    Q_OBJECT

public:
    explicit Md19Client(QObject *parent = nullptr);
    void startDev();
    void stopDev();
    void setTemp(unsigned short temp, unsigned short xishiV);
    void readTemp();
    bool getStart() { return beStart;}
    bool getSwitch() { return beRemoteStart;}
    void setRemoteSwitch(bool swi);
    void readParam();

signals:
    void sigData(int cmd, QVariant var, QVariant var1, int reg);
    void sigParam(int func, QMap<QString,QString> param);

private slots:
    void onDataReady();
    void onDataRecv();

private:
    void parseContent(QByteArray content);
    int writeRegN(char cmd, char addrH, char addL, int regNum, char *writeData, int dataLen);
    int writeRegS(char cmd, char addrH, char addL, char writeDataH, char writeDataL);
    int readReg(char cmd, char addrH, char addL, int regNum);
    int writeMyData(const char *data, int dataLen);
    bool beStart = false;
    bool beRemoteStart = false;

    MdDataHeader header;
};

#endif // MD19CLIENT_H
