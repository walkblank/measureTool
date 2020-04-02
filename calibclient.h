#ifndef CALIBCLIENT_H
#define CALIBCLIENT_H

#include <QObject>
#include <QTcpSocket>

class CalibClient : public QObject
{
    Q_OBJECT
public:
    CalibClient();
    void AttachSocket(QTcpSocket *sock);
    void setClientType(QString type) {clientType = type;}
    QString getClientType() { return clientType;}

    int sendCpcValue(int cmd, int value);
    int getCpcValue(int cmd);

    qint64 writeData(const char *data);
    void close();

signals:
    void sigReadData(QString type, QString value);

private slots:
    void onDataReady();

private:
    QTcpSocket *sock;
    QString clientType = QString();
    QByteArray dataRemain;
};

#endif // CALIBCLIENT_H
