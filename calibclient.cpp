#include "calibclient.h"

CalibClient::CalibClient()
{
}

void CalibClient::AttachSocket(QTcpSocket *sock)
{
    this->sock = sock;
    connect(sock, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

qint64 CalibClient::writeData(const char *data)
{
    return sock->write(data);
}

void CalibClient::close()
{
    return sock->close();
}

void CalibClient::onDataReady()
{
    qDebug()<<sock->readAll();
}
