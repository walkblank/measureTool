#ifndef MEASURESERVER_H
#define MEASURESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class MeasureServer : public QObject
{
    Q_OBJECT
public:
    MeasureServer();

private slots:
    void onNewConnection();

private:
    QTcpServer *serverSocket;
    QMap<QString, QTcpSocket*> clientList;
};

#endif // MEASURESERVER_H
