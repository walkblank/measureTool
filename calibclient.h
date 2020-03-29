#ifndef CALIBCLIENT_H
#define CALIBCLIENT_H

#include <QObject>
#include <QTcpSocket>

class CalibClient : public QObject
{
    Q_OBJECT
public:
    CalibClient();
    void AttachSocket(QTcpSocket *sock);

private slots:
    void onDataReady();

private:
    QTcpSocket *sock;
};

#endif // CALIBCLIENT_H
