#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QHostInfo>

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
    addrList["cpc"] = settings->value("cpcaddr").toString();
    addrList["smps"] = settings->value("smpsaddr").toString();
    addrList["test"]  = settings->value("testaddr").toString();
    server->setClientMap(addrList);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    connect(server, SIGNAL(sigOnConnect(QString)), this, SLOT(onSigClientConn(QString)));
    connect(server, SIGNAL(sigDisconnect(QString)), this, SLOT(onSigClientDisconn(QString)));

    initChartsView();
    loadSettings();
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
            ui->localAddr->setText(addr.toString());
            break;
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
}

void MainWindow::onTimerTimeout()
{
    static int cnt = -1;
    if(cnt < 600)
    {
        pt.prepend(QRandomGenerator::global()->bounded(1.00));
        pt1.prepend(QRandomGenerator::global()->bounded(1.00));
        cnt ++;
    }
    else
    {
        cnt ++;
        pt.prepend(QRandomGenerator::global()->bounded(1.00));
        pt1.prepend(QRandomGenerator::global()->bounded(1.00));
        pt.removeAt(pt.size());
        pt1.removeAt(pt1.size());
    }
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
    for(int i = 0; i < pt.size(); i ++)
    {
        cpc1slineSeries->append(i, pt[i]);
        cpc10slineSeries->append(i, pt1[i]);
    }

}

void MainWindow::initChartsView()
{
    cpc1slineSeries = new QLineSeries();
    cpc10slineSeries  = new QLineSeries();
    upperCalib = new QLineSeries();
    lowerCalib = new QLineSeries();
    upperCalib->setPen(QPen(QBrush(Qt::red), 2, Qt::DashLine));
    lowerCalib->setPen(QPen(QBrush(Qt::green), 2, Qt::DashLine));

    cpc1slineSeries->setUseOpenGL(true);
    cpc10slineSeries->setUseOpenGL(true);

    cpc1slineSeries->setName("CPC(1s)");
    cpc10slineSeries->setName("CPC(10s)");

    upperCalib->setName("上限");
    lowerCalib->setName("下限");

    m_chartView = new QChartView(m_chart);
    m_chartView->setMinimumSize(1000, 600);

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 599);
    xAxis->setTickCount(30);
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
    m_chart->addSeries(upperCalib);
    m_chart->addSeries(lowerCalib);

    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);

    cpc1slineSeries->attachAxis(yAxis);
    cpc1slineSeries->attachAxis(xAxis);

    cpc10slineSeries->attachAxis(yAxis);
    cpc10slineSeries->attachAxis(xAxis);

    lowerCalib->attachAxis(xAxis);
    lowerCalib->attachAxis(yAxis);
    upperCalib->attachAxis(xAxis);
    upperCalib->attachAxis(yAxis);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartViewLayout->addWidget(m_chartView);
}

void MainWindow::on_startBtn_clicked()
{
    timer->start(200);
}

void MainWindow::on_menuBtn_clicked()
{

}

void MainWindow::on_startCalibBtn_clicked()
{
    //clear all lines;
    //reset connections;
    //reset server settings, eg. port;
    // set smps and cpc to classifier mode
    // show cpc1s and cps10s lineseries;
    qDebug()<<connList.size();
    qDebug()<<connList.keys();
    foreach(QString key, connList.keys())
    {
        connList[key]->writeData("jjsddfda");
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

void MainWindow::on_cnValue_textChanged(const QString &arg1)
{

}

void MainWindow::on_averTime_textChanged(const QString &arg1)
{

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

void MainWindow::on_setMeter_editingFinished()
{
    //send to smps  device   <sendVal140=100;>
}

void MainWindow::onSigClientConn(QString ip)
{
    qDebug()<<"connect signal" << ip;
}

void MainWindow::onSigClientDisconn(QString ip)
{
    qDebug()<<"disconnect signal" << ip;
}
