#ifndef CALIBCLIENT_H
#define CALIBCLIENT_H

#include <QObject>
#include <QTcpSocket>

class CalibClient : public QObject
{
    Q_OBJECT
public:
    CalibClient();
    CalibClient(QString s);
    void AttachSocket(QTcpSocket *sock);

    void setClientType(QString type) {clientType = type;}
    QString getClientType() { return clientType;}

    int getValue(QList<int> vals);
    int setValue(QMap<int,int> valueSet);

    void setMode(QList<int> modes);

    void connectToHost(QString host, int port);
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
