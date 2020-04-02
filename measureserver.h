#ifndef MEASURESERVER_H
#define MEASURESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <calibclient.h>

class MeasureServer : public QObject
{
    Q_OBJECT
public:
    MeasureServer(QMap<QString, CalibClient*> &connList, int port =11231);
    void setListenPort(int port);
    void setClientMap(QMap<QString,QString>);

signals:
    void sigOnConnect(QString ip);
    void sigDisconnect(QString ip);
    void sigOnClientData(QString type, QString value);

private slots:
    void onNewConnection();
    void onClientDisconnect();

private:
    QTcpServer *serverSocket;
    int listenPort;
    QMap<QString, CalibClient*> &clientList;
    QMap<QString, QString> clientMap;
};

#endif // MEASURESERVER_H
