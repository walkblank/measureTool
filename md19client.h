#ifndef MD19CLIENT_H
#define MD19CLIENT_H

#include <QTcpSocket>


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
    void setRemoteSwitch(bool swi);

signals:
    void sigData(int cmd, QVariant var, QVariant var1, int reg = 0);

private slots:
    void onDataReady();

private:
    int writeRegN(char cmd, char addrH, char addL, int regNum, char *writeData, int dataLen);
    int writeRegS(char cmd, char addrH, char addL, char writeDataH, char writeDataL);
    int readReg(char cmd, char addrH, char addL, int regNum);
    int writeMyData(const char *data, int dataLen);
    bool beStart = false;

};

#endif // MD19CLIENT_H
