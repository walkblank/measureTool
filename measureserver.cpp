#include "measureserver.h"

MeasureServer::MeasureServer(QMap<QString, CalibClient*> &connList, int port)
    : listenPort(port), clientList(connList)
{
    serverSocket = new QTcpServer(this);
    serverSocket->listen(QHostAddress::AnyIPv4, listenPort);
    connect(serverSocket, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void MeasureServer::setListenPort(int port)
{
    if(port != listenPort)
    {
        serverSocket->close();
        serverSocket->listen(QHostAddress::AnyIPv4, port);
        listenPort = port;
        foreach(QString key, clientList.keys())
        {
            clientList[key]->close();
            delete clientList[key];
        }
        clientList.clear();
    }
}

void MeasureServer::setClientMap(QMap<QString, QString> map)
{
    qDebug()<< "set client map";
    if(!clientMap.isEmpty())
    {
        qDebug()<<"clear old clients";
        foreach(QString key, map.keys())
        {
            if(map[key] != clientMap[key])
            {
                if(clientList.contains(clientMap[key]))
                {
                    clientList[key]->close();
                    delete clientList[key];
                    clientList.remove(key);
                    qDebug()<<"clear " << key;
                }
                else
                {
                    qDebug()<<"no client in list";
                }
            }
        }
    }
    clientMap = map;
}

void MeasureServer::onClientDisconnect()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    qDebug()<<"disconnect" << sock->peerAddress().toString();
    clientList[sock->peerAddress().toString()]->close();
    emit sigDisconnect(sock->peerAddress().toString());
}

void MeasureServer::onNewConnection()
{
    qDebug()<<"new Connect";
    CalibClient *client = new CalibClient();
    QTcpSocket *socket = serverSocket->nextPendingConnection();
    client->AttachSocket(socket);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onClientDisconnect()));
    qDebug()<<socket->peerName();
    qDebug()<<socket->peerPort();
    qDebug()<<socket->peerAddress();
//    if(clientList.contains(socket))
    clientList[socket->peerAddress().toString()] = client;
    emit sigOnConnect(socket->peerAddress().toString());
}
