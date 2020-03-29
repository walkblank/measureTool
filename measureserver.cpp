#include "measureserver.h"

MeasureServer::MeasureServer(QMap<QString, CalibClient*> &connList)
    : clientList(connList)
{
    serverSocket = new QTcpServer(this);
    serverSocket->listen(QHostAddress::AnyIPv4, 11231);
    connect(serverSocket, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}


void MeasureServer::setListenPort(int port)
{
    serverSocket->close();
    serverSocket->listen(QHostAddress::AnyIPv4, port);
}

void MeasureServer::onClientDisconnect()
{
    QTcpSocket *sock = static_cast<QTcpSocket*>(sender());
    qDebug()<<"disconnect" << sock->peerAddress().toString();
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
}
