#include "measureserver.h"

MeasureServer::MeasureServer(QMap<QString, CalibClient*> &connList, int port)
    : listenPort(port), clientList(connList)
{
    serverSocket = new QTcpServer(this);
    serverSocket->listen(QHostAddress::AnyIPv4, listenPort);
    connect(serverSocket, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

MeasureServer::MeasureServer(int port)
    : listenPort(port)
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
            emit sigDisconnect(key);
        }
        clientList.clear();
    }
}

void MeasureServer::setClientMap(QMap<QString, QString> map)
{
    qDebug()<< "set client map";
    qDebug() <<map<< clientMap;
    if(!clientMap.isEmpty())
    {
        qDebug()<<"clear old clients";
        foreach(QString key, map.keys())
        {
            if(map[key] != clientMap[key])
            {
                qDebug()<<"change key" << key << clientMap[key];
                if(clientList.contains(clientMap[key]))
                {
                    clientList[clientMap[key]]->close();
                    delete clientList[clientMap[key]];
                    clientList.remove(clientMap[key]);
                    emit sigDisconnect(map[key]);
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
    qDebug() <<map<< clientMap;
}

void MeasureServer::onClientDisconnect()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    qDebug()<<"disconnect" << sock->peerAddress().toString();
    QString key = sock->peerAddress().toString();
    clientList[key]->close();
    delete clientList[key];
    clientList.remove(key);
    emit sigDisconnect(sock->peerAddress().toString());
}

void MeasureServer::onNewConnection()
{
    qDebug()<<"new Connect";
    QTcpSocket *socket = serverSocket->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), this, SLOT(onClientDisconnect()));
    qDebug()<<socket->peerAddress();
    QString address = socket->peerAddress().toString();

    CalibClient *client = new CalibClient("simu", true);
    client->AttachSocket(socket);
    connect(client, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SIGNAL(sigOnClientData(QString, QMap<QString,QString>)));
    connect(client, SIGNAL(sigSetRet(QString,QString, QMap<QString,QString>)),
            this, SIGNAL(sigSetRet(QString,QString, QMap<QString,QString>)));
    clientList[socket->peerAddress().toString()] = client;
    emit sigOnConnect(socket->peerAddress().toString());
}
