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
    MeasureServer(QMap<QString, CalibClient*> &connList);
    void setListenPort(int port);

signals:
    void sigOnConnect(QString ip);

private slots:
    void onNewConnection();
    void onClientDisconnect();

private:
    QTcpServer *serverSocket;
    QMap<QString, CalibClient*> &clientList;
};

#endif // MEASURESERVER_H
