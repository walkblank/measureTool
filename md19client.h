#ifndef MD19CLIENT_H
#define MD19CLIENT_H

#include <QTcpSocket>


class Md19Client : public QTcpSocket
{
    Q_OBJECT

public:
    explicit Md19Client(QObject *parent = nullptr);
};

#endif // MD19CLIENT_H
