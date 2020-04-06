#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QHostInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_chart(new QChart())
{
    ui->setupUi(this);

    int port = 11231;
    settings = new QSettings("setting.ini", QSettings::IniFormat);
    if(settings->contains("port"))
        port = settings->value("port").toUInt();
    server = new MeasureServer(connList, port);
    timer = new QTimer();
    autoTimer = new QTimer();

    addrList["cpc"] = settings->value("cpcaddr").toString();
    addrList["smps"] = settings->value("smpsaddr").toString();
    addrList["test"]  = settings->value("testaddr").toString();

    addrLineList["cpc"] = ui->cpcAddr;
    addrLineList["smps"] = ui->smpsAddr;
    addrLineList["test"] = ui->testDevAddr;

    server->setClientMap(addrList);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    connect(autoTimer, SIGNAL(timeout()), this, SLOT(onAutoTimeout()));

    connect(server, SIGNAL(sigOnConnect(QString)), this, SLOT(onSigClientConn(QString)));
    connect(server, SIGNAL(sigDisconnect(QString)), this, SLOT(onSigClientDisconn(QString)));
    connect(server, SIGNAL(sigOnClientData(QString,QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
    connect(server, SIGNAL(sigSetRet(QString,QString, QMap<QString,QString>)),
            this, SLOT(onClientRet(QString,QString, QMap<QString,QString>)));
    initChartsView();
    loadSettings();

    model = new DatacompareModel();
    compareWindow = new DatacompareWindow();
    compareWindow->setWindowTitle("Compare Table");
    compareWindow->setModel(model);

    pointMaps["cpc"] = cpcPt1;
    pointMaps["test"] = cpcTest;
    pointMaps["cpc10"] = cpcPt10;
    simuPage = new ClientSimuPage();
    settingWindow = new ClientSettingWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getLocalIpAddr()
{
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    qDebug()<< QHostInfo::localHostName();
    foreach(QHostAddress addr, info.addresses())
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug()<<addr.toString();
            ui->localAddrList->addItem(addr.toString());
        }
    }
}

bool MainWindow::checkIp(QString ip)
{
    QRegExp rx2("^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$");
    if(!rx2.exactMatch(ip))
        return false;
    return true;
}

void MainWindow::loadSettings()
{
    getLocalIpAddr();
    if(settings->contains("upper"))
        ui->upperValue->setText(settings->value("upper").toString());
    if(settings->contains("lower"))
        ui->lowerValue->setText(settings->value("lower").toString());
    if(settings->contains("cpcaddr"))
        ui->cpcAddr->setText(settings->value("cpcaddr").toString());
    if(settings->contains("smpsaddr"))
        ui->smpsAddr->setText(settings->value("smpsaddr").toString());
    if(settings->contains("testaddr"))
        ui->testDevAddr->setText(settings->value("testaddr").toString());
    if(settings->contains("port"))
        ui->listenPort->setText(settings->value("port").toString());
    if(settings->contains("diameter"))
        ui->setMeter->setText(settings->value("diameter").toString());
    if(settings->contains("avtime"))
        ui->averTime->setText(settings->value("avtime").toString());
}

void MainWindow::onAutoTimeout()
{
    QList<QString> channels;
    channels << "25";
    connList[addrList["cpc"]]->getValue(channels);
    connList[addrList["test"]]->getValue(channels);
}

void MainWindow::onTimerTimeout()
{

//    static int cnt = -1;
//    if(cnt < 600)
//    {
//        pt.prepend(QRandomGenerator::global()->bounded(1.00));
//        pt1.prepend(QRandomGenerator::global()->bounded(1.00));
//        cnt ++;
//    }
//    else
//    {
//        cnt ++;
//        pt.prepend(QRandomGenerator::global()->bounded(1.00));
//        pt1.prepend(QRandomGenerator::global()->bounded(1.00));
//        pt.removeAt(pt.size());
//        pt1.removeAt(pt1.size());
//    }
//    cpc1slineSeries->clear();
//    cpc10slineSeries->clear();
//    for(int i = 0; i < pt.size(); i ++)
//    {
//        cpc1slineSeries->append(i, pt[i]);
//        cpc10slineSeries->append(i, pt1[i]);
//    }
    qDebug()<<"request cpc data";
    QList<QString> getChannels;
    getChannels << "25";
    connList[addrList["cpc"]]->getValue(getChannels);
    connList[addrList["test"]]->getValue(getChannels);
    getChannels.clear();
    getChannels << "21" << "26" << "27" << "28";
    connList[addrList["smps"]]->getValue(getChannels);
}

void MainWindow::initChartsView()
{
    cpc1slineSeries = new QLineSeries();
    cpc10slineSeries  = new QLineSeries();
    testDevLineSeries = new QLineSeries();

    upperCalib = new QLineSeries();
    lowerCalib = new QLineSeries();
    upperCalib->setPen(QPen(QBrush(Qt::red), 2, Qt::DashLine));
    lowerCalib->setPen(QPen(QBrush(Qt::green), 2, Qt::DashLine));

    cpc1slineSeries->setUseOpenGL(true);
    cpc10slineSeries->setUseOpenGL(true);
    testDevLineSeries->setUseOpenGL(true);

    cpc1slineSeries->setName("CPC(1s)");
    cpc10slineSeries->setName("CPC(10s)");
    testDevLineSeries->setName("testDev");

    upperCalib->setName("上限");
    lowerCalib->setName("下限");

    m_chartView = new QChartView(m_chart);
    m_chartView->setMinimumSize(1200, 600);

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 600);
    xAxis->setTickCount(31);
    xAxis->setMinorTickCount(1);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText("time(s)");

    QValueAxis *yAxis = new QValueAxis;
    yAxis->setRange(-1, 1);
    yAxis->setTickCount(11);
    yAxis->setLabelFormat("%.2f");
    yAxis->setMinorTickCount(1);
    yAxis->setTitleText("value");

    m_chart->addSeries(cpc1slineSeries);
    m_chart->addSeries(cpc10slineSeries);
    m_chart->addSeries(testDevLineSeries);
    m_chart->addSeries(upperCalib);
    m_chart->addSeries(lowerCalib);

    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);

    cpc1slineSeries->attachAxis(yAxis);
    cpc1slineSeries->attachAxis(xAxis);

    cpc10slineSeries->attachAxis(yAxis);
    cpc10slineSeries->attachAxis(xAxis);

    testDevLineSeries->attachAxis(xAxis);
    testDevLineSeries->attachAxis(yAxis);

    lowerCalib->attachAxis(xAxis);
    lowerCalib->attachAxis(yAxis);
    upperCalib->attachAxis(xAxis);
    upperCalib->attachAxis(yAxis);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartViewLayout->addWidget(m_chartView);
}

void MainWindow::on_startBtn_clicked()
{
    cpcPt1.clear();
    cpcPt10.clear();
    cpcTest.clear();
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
    testDevLineSeries->clear();
    autom = true;
    calib = false;
    timer->stop();
    autoTimer->start(1000);
}

void MainWindow::on_menuBtn_clicked()
{
    settingWindow->show();
}

void MainWindow::on_startCalibBtn_clicked()
{
    //clear all lines;
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
    averageTime = ui->averTime->text().toInt();
    qDebug()<<"times" << 600/averageTime;

    calib = true;
    //reset connections;
    //reset server settings, eg. port;
    // set smps and cpc to classifier mode
    // show cpc1s and cps10s lineseries;
    qDebug()<<connList.size();
    qDebug()<<connList.keys();

    if(connList.contains(addrList["cpc"]))
    {
        connList[addrList["cpc"]]->enterClassifierMode(ui->setMeter->text());
        connList[addrList["smps"]]->enterClassifierMode(ui->setMeter->text());
        connList[addrList["test"]]->enterClassifierMode(ui->setMeter->text());
    }
}

void MainWindow::on_upperValue_textChanged(const QString &arg1)
{
    bool ret;
    qreal v = arg1.toDouble(&ret);
    if(ret)
    {
        upperCalib->clear();
        upperCalib->append(0, v);
        upperCalib->append(600, v);
    }
    settings->setValue("upper", v);

}

void MainWindow::on_lowerValue_textChanged(const QString &arg1)
{
    bool ret;
    qreal v = arg1.toDouble(&ret);
    if(ret)
    {
        lowerCalib->clear();
        lowerCalib->append(0, v);
        lowerCalib->append(600, v);
    }
    settings->setValue("lower", v);
}

void MainWindow::on_averTime_textChanged(const QString &arg1)
{
    bool ret;
    if(arg1.toInt(&ret))
        settings->setValue("avtime", arg1);
}

void MainWindow::on_saveBtn_clicked()
{
   if(!checkIp(ui->cpcAddr->text())
           || !checkIp(ui->smpsAddr->text())
           || !checkIp(ui->testDevAddr->text()))
   {
        QMessageBox::warning(this, "提示", tr("invalid ip address"), QMessageBox::Ok);
        return;
   }
   else
   {
       settings->setValue("cpcaddr", ui->cpcAddr->text());
       settings->setValue("smpsaddr", ui->smpsAddr->text());
       settings->setValue("testaddr", ui->testDevAddr->text());
   }

   int port = ui->listenPort->text().toInt();
   if(port < 0 || port > 65535)
   {
        QMessageBox::warning(this, "提示", tr("invalid listen port"), QMessageBox::Ok);
        return;
   }
   else
   {
       settings->setValue("port", ui->listenPort->text());
   }

   server->setListenPort(port);
   addrList["cpc"] = ui->cpcAddr->text();
   addrList["smps"] = ui->smpsAddr->text();
   addrList["test"]  = ui->testDevAddr->text();
   server->setClientMap(addrList);
}

void MainWindow::onSigClientConn(QString ip)
{
    qDebug()<<"connect signal" << ip;
    foreach(QString str, addrList.keys())
        if(addrList[str] == ip)
            addrLineList[str]->setStyleSheet("background-color: rgb(0, 255, 127);");
}

void MainWindow::onSigClientDisconn(QString ip)
{
    qDebug()<<"disconnect signal" << ip;
    foreach(QString str, addrList.keys())
    {
        if(addrList[str] == ip)
            addrLineList[str]->setStyleSheet("background-color: rgb(255, 255, 127);");
    }
}

void MainWindow::onClientData(QString client, QMap<QString,QString> data)
{
    qDebug()<<"onData" << client << data;
    if(client == "smps")
    {
        ui->smpsVal21->setText(data["21"]);
        ui->smpsVal26->setText(data["26"]);
        ui->smpsVal27->setText(data["27"]);
        ui->smpsVal28->setText(data["28"]);
    }

    if(client == "cpc")
    {
        if(data.contains("25"))
        {
            double value = data["25"].toDouble();
            static QList<double> tmpPt;
            ui->cnValue->setText(data["25"]);

            if(autom)
            {
                datetimeList.append(QDateTime::currentDateTime().toString("yyyy-MMdd-hh:mm:ss"));
                cpcVList.append(value);
                if(cpcVList.size() >= testVList.size() && errVList.size() < testVList.size())
                {
                    errVList.append(cpcVList[errVList.size()] - testVList[errVList.size()]);
                }
            }

            tmpPt.prepend(value);
            if(cpcPt1.size() < 600)
            {
                cpcPt1.prepend(value);
            }
            else
            {
                cpcPt1.prepend(value);
                cpcPt1.removeAt(cpcPt1.size());
            }

            double sum = 0;
            if(tmpPt.size() == averageTime)
            {
                foreach(double v, tmpPt)
                    sum += v;
                sum = sum/averageTime;
                if(cpcPt10.size() < 600/averageTime)
                {
                    cpcPt10.prepend(sum);
                }
                else
                {
                    cpcPt10.prepend(sum);
                    cpcPt10.removeAt(cpcPt10.size());
                }
                tmpPt.clear();
                qDebug()<< cpcPt10;
            }

            cpc1slineSeries->clear();
            cpc10slineSeries->clear();
            for(int i = 0; i < cpcPt1.size(); i ++)
            {
                cpc1slineSeries->append(i, cpcPt1[i]);
            }

            if(!autom)
            {
                cpc10slineSeries->clear();
                for(int i = 0; i < cpcPt10.size(); i ++)
                    cpc10slineSeries->append(i*averageTime+cpcPt1.size()-averageTime*cpcPt10.size(), cpcPt10[i]);

                if(cpcPt10.size() > 0 && sum > ui->lowerValue->text().toDouble() && sum < ui->upperValue->text().toDouble())
                {
                    ui->startBtn->setDisabled(false);
//                    calib = false;
                }
            }
        }
    }

    if(client == "test")
    {
        if(data.contains("25"))
        {
            double value = data["25"].toDouble();
            qDebug()<<"on testDev data";
            if(autom)
            {
                //testList.append(new QStandardItem(data["25"]));
                testVList.append(value);
                if(cpcVList.size() >= testVList.size() && errVList.size() < testVList.size())
                {
                    errVList.append(cpcVList[errVList.size()] - testVList[errVList.size()]);
                }
            }
            if(!calib)
            {
                if(cpcTest.size() < 600 )
                {
                    cpcTest.prepend(value);
                }
                else
                {
                    cpcTest.prepend(value);
                    cpcTest.removeAt(cpcTest.size());
                }

                testDevLineSeries->clear();
                for(int i = 0; i < cpcTest.size(); i ++)
                {
                    testDevLineSeries->append(i, cpcTest[i]);
                }
            }
        }
    }
}

void MainWindow::onClientRet(QString type, QString ret, QMap<QString,QString> setValues)
{
    qDebug()<<"on Ret" << type << ret << setValues;
    if(setValues.contains("201") && setValues["201"] == "4")
    {
        qDebug()<<"type" <<type <<"enter class mode" << ret;
        if(ret == "ok")
            enterClsDevs.append(type);
    }

    if(enterClsDevs.contains("cpc") && enterClsDevs.contains("smps") && enterClsDevs.contains("test"))
    {
        enterClsDevs.clear();
        timer->start(1000);
    }

    if(enterAutoDevs.contains("cpc") && enterAutoDevs.contains("smps") && enterAutoDevs.contains("test"))
    {
        enterAutoDevs.clear();
    }
}

void MainWindow::on_clientListBtn_clicked()
{
    simuPage->show();
}

void MainWindow::on_setMeter_textChanged(const QString &arg1)
{
    bool ret;
    ret = arg1.toInt(&ret);
    if(ret)
        settings->setValue("diameter", arg1);
}

void MainWindow::on_tableBtn_clicked()
{
    model->removeRows(0, model->rowCount());
    for(int i = 0; i < errVList.size(); i ++)
    {
        if(i > testVList.size() || i > cpcVList.size())
            break;
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(datetimeList[i]) << new QStandardItem(QString("%1").arg(cpcVList[i]))
                 << new QStandardItem(QString("%1").arg(testVList[i])) << new QStandardItem(QString("%1").arg(errVList[i]));

        model->appendRow(rowItems);
    }

    compareWindow->show();
}
