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
    qint64 writeData(const char *data);
    void close();

private slots:
    void onDataReady();

private:
    QTcpSocket *sock;
};

#endif // CALIBCLIENT_H
