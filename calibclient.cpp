#include "calibclient.h"

CalibClient::CalibClient()
{
}

void CalibClient::AttachSocket(QTcpSocket *sock)
{
    this->sock = sock;
    connect(sock, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

void CalibClient::close()
{
    return sock->close();
}

int CalibClient::getValue(QList<int> vals)
{
    QString valContent, sendStr;
    foreach(int val, vals)
        valContent.append(QString("%1;").arg(val));
    sendStr = QString("<getVal%1>\r\n");
    qDebug()<<"get val" << sendStr;
    return sock->write(sendStr.toStdString().c_str());
}

int CalibClient::setValue(QMap<int,int> valueSet)
{
    QString valContent, sendStr;
    foreach(int key, valueSet.keys())
        valContent.append(QString("%1=%2;").arg(key).arg(valueSet[key]));
    sendStr = QString("<sendVal%1>\r\n").arg(valContent);
    qDebug()<<"set val" << sendStr;
    return sock->write(sendStr.toStdString().c_str());
}

void CalibClient::onDataReady()
{
    QByteArray readData = sock->readAll();
    readData.replace("test", "kkk");
    readData.lastIndexOf(QByteArray("\r\n"));
}
