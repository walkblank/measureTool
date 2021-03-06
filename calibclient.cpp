#include "calibclient.h"

CalibClient::CalibClient(QString type)
{
    sock = new QTcpSocket(this);
    connect(sock, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(onDisConnected()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}


CalibClient::CalibClient(QString s, bool beSimu)
    :beSimu(true)
{
    qDebug()<<s;
    sock = new QTcpSocket(this);
    connect(sock, SIGNAL(readyRead()), this, SLOT(onDataReady()));
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

int CalibClient::state()
{
    return sock->state();
}

void CalibClient::connectToHost(QString host, int port)
{
    sock->connectToHost(host, port);
}

void CalibClient::disconnect()
{
    sock->disconnectFromHost();
}

void CalibClient::enterClassifierMode(QString diameter)
{
    QMap<QString,QString> cmdMap;
    cmdMap["201"] = "4";
    if(getClientType() == "smps")
    {
        cmdMap["140"] = diameter;
    }
    setValue(cmdMap);
}

void CalibClient::enterSmpsClassifierMode(QString diameter)
{
    QMap<QString,QString> cmdMap;
    cmdMap["201"] = "4";
    cmdMap["140"] = diameter;

    setValue(cmdMap);
}

void CalibClient::setMeter(QString diameter)
{
    QMap<QString,QString> cmdMap;
    cmdMap["140"] = diameter;
    setValue(cmdMap);
}

void CalibClient::enterAutoMode()
{
    QMap<QString,QString> cmdMap;
    cmdMap["201"] = "1";
    setValue(cmdMap);
}

int CalibClient::getValue(QList<QString> vals)
{
    if(state() != QAbstractSocket::ConnectedState)
        return 0;
    QStringList valList;
    QString sendStr;
    foreach(QString val, vals)
        valList.append(val);
    sendStr = QString("<getVal%1>\r\n").arg(valList.join(";"));
    qDebug()<<"get val" << sendStr;
    return sock->write(sendStr.toStdString().c_str());
}

int CalibClient::setValue(QMap<QString,QString> valueSet)
{
    if(state() != QAbstractSocket::ConnectedState)
        return 0;
    QStringList valList;
    int sendLen;
    setMode(valueSet.keys());
    QStringList valSet;
    QString sendStr;
    foreach(QString key, valueSet.keys())
        valSet.append(QString("%1=%2").arg(key).arg(valueSet[key]));
    sendStr = QString("<sendVal%1>\r\n").arg(valSet.join(";"));
    qDebug()<<"set val" << sendStr;
    sendValueSet = valueSet;
    sendLen = sock->write(sendStr.toStdString().c_str());
    qDebug()<<"sendLen" << sendLen;
    return sendLen;
}

void CalibClient::onConnected()
{
    emit sigConnected();
}

void CalibClient::onDisConnected()
{
    emit sigDisConnected();
}

void CalibClient::onDataReady()
{
    if(beSimu)
    {
        simuDataProcess();
    }
    else
    {
        if(getClientType() == "ak")
            akDataProcess();
        else
            commDataProcess();
    }
}

void CalibClient::akSendQueryCommand()
{
    QByteArray sendData;
    sendData.append(0x2);
    sendData.append(' ');
    sendData.append("ACPC");
    sendData.append(' ');
    sendData.append("K0");
    sendData.append(0x3);

    sock->write(sendData);
}

void CalibClient::akDataProcess()
{
    static QByteArray dataLeft = QByteArray();
    QByteArray readData = sock->readAll();
    readData.prepend(dataLeft);

    while(readData.contains(0x3))
    {
        QMap<QString,QString> valMap;
        int secEndPos = readData.indexOf(0x3);
        QByteArray secData = readData.mid(0, secEndPos+1);

        secData.remove(secData.size()-1, 1);
        secData.remove(0, 1);
        QList<QByteArray> dataArray;
        dataArray = secData.split(' ');
        for(int i = 5; i < dataArray.size(); i ++)
            valMap[QString("%1").arg(i-2)] = QString(dataArray[i]);
//        valMap["cn"] = QString(dataArray[5]);
//        valMap["flowRate"] = QString(dataArray[11]);
//        valMap["pressure"] = QString(dataArray[10]);
        emit sigReadData(getClientType(), valMap);
        readData = readData.mid(secEndPos+1, -1);
    }

    dataLeft = readData;
}

void CalibClient::commDataProcess()
{
    static QString strLeft = QString();
    QString data = sock->readAll();
    qDebug()<<"commRecv" << data;
    data.prepend(strLeft);
    strLeft.clear();

    while(data.contains("\r\n"))
    {
        int secEndpos = data.indexOf("\r\n");
//        qDebug()<<"comm sec end pos" << secEndpos;
        QString sec = data.mid(0, secEndpos);
        if(sec.contains("sendVal"))
        {
            int startPos = sec.indexOf("<");
            int endPos = sec.indexOf(">");
            QMap<QString,QString> readVals;
            QString content = sec.mid(startPos+1, endPos-1).replace("sendVal ", "");
            qDebug()<<"comm content" << content;
            QStringList contentList = content.split(";");
            foreach(QString singleCont, contentList)
            {
//                qDebug()<<"comm singleCont" << singleCont;
                QStringList minContent = singleCont.split("=");
//                qDebug()<<"comm cmd" << minContent[0];
//                qDebug()<<"comm value" << minContent[1];
                readVals[minContent[0]] = minContent[1];
            }

            emit sigReadData(getClientType(), readVals);
        }
        else if(sec.contains("sendVal"))
        {
//            qDebug()<<"commRecv sendVal";
        }
        else if (sec.contains("ok") || sec.contains("fail"))
        {
//            qDebug()<<"comm set ret" << sec;
            emit sigSetRet(getClientType(), sec.contains("ok")? "ok" : "fail", sendValueSet);
        }
        data = data.mid(secEndpos+2, -1);
    }
    strLeft = data;
}

void CalibClient::simuDataProcess()
{
    static QString strLeft = QString();
    QString data = sock->readAll();
    qDebug()<<"simuRecv" << data;
    data.prepend(strLeft);

    while(data.contains("\r\n"))
    {
        int secEndpos = data.indexOf("\r\n");
        QString sec = data.mid(0, secEndpos);
//        qDebug()<<"simu sec" << sec;
        if(sec.contains("sendVal")) // set dev value
        {
//            qDebug()<<"simuRecv sendVal";
            int startPos = sec.indexOf("<");
            int endPos = sec.indexOf(">");
            QString content = sec.mid(startPos+1, endPos-1).replace("sendVal", "");
//            qDebug()<<"content" << content;
            QStringList contentList = content.split(";");
            foreach(QString singleCont, contentList)
            {
//                qDebug()<<"simu ingleCont" << singleCont;
                QStringList minContent = singleCont.split("=");
//                qDebug()<<"simu cmd" << minContent[0];
//                qDebug()<<"simu value" << minContent[1];
                devValueSet[minContent[0]] = minContent[1];
            }
            sock->write("<ok>01\r\n");
        }
        else if(data.contains("getVal")) // read dev value
        {
//            qDebug()<<"simuRecv getVal";
            int startPos = sec.indexOf("<");
            int endPos = sec.indexOf(">");
            QString content = sec.mid(startPos+1, endPos-1).replace("getVal", "");
            QStringList cmdList = content.split(";");
            QStringList sendValList;
            foreach(QString singleCmd, cmdList)
            {
//                qDebug()<<"simu singleGetVal" << singleCmd;
                if(devValueSet.contains(singleCmd))
                {
                    sendValList.append(QString("%1=%2").arg(singleCmd).arg(devValueSet[singleCmd]));
                }
                else if(singleCmd == "27" || singleCmd == "28")
                {
                    sendValList.append(QString("%1=%2").arg(singleCmd).arg(QRandomGenerator::global()->bounded(1.00)+10));
                }
                else if(singleCmd == "0")
                {
                    sendValList.append(QString("%1=%2").arg(singleCmd).arg(QRandomGenerator::global()->bounded(2)));
                }
                else
                {
                    sendValList.append(QString("%1=%2").arg(singleCmd).arg(QRandomGenerator::global()->bounded(50)+
                                                                           QRandomGenerator::global()->bounded(1.00)));
//                    sendValList.append(QString("%1=%2").arg(singleCmd).arg(50+
//                                                                           QRandomGenerator::global()->bounded(1.00)));
                }
            }
//            qDebug()<<"simu sendvals" << sendValList.join(";");
//            QString sendStr = QString("<sendVals%1>07\r\n").arg(sendValList.join(";"));
            QString sendStr = QString("<sendVal %1>07\r\n").arg(sendValList.join(";"));
            sock->write(sendStr.toStdString().c_str());
        }

        data = data.mid(secEndpos+2, -1);
    }
}
