#include "calibclient.h"

CalibClient::CalibClient()
{
}

void CalibClient::AttachSocket(QTcpSocket *sock)
{
    this->sock = sock;
    connect(sock, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

void CalibClient::onDataReady()
{
    qDebug()<<sock->readAll();
}
