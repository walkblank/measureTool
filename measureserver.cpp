#include "measureserver.h"

MeasureServer::MeasureServer()
{
    serverSocket = new QTcpServer(this);
    serverSocket->listen(QHostAddress::AnyIPv4, 8080);
    connect(serverSocket, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}


void MeasureServer::onNewConnection()
{
    qDebug()<<"new Connect";
    QTcpSocket *socket = serverSocket->nextPendingConnection();
    qDebug()<<socket->peerName();
    qDebug()<<socket->peerPort();
    qDebug()<<socket->peerAddress();
    clientList[socket->peerAddress().toString()] = socket;
}
