#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QHostInfo>
#include <QDateTime>

MainWindow::MainWindow(QString dev, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_chart(new QChart())
{
    ui->setupUi(this);
    deviceName = dev;
    setWindowTitle(QString("%1设备界面").arg(dev));
    settings = new QSettings("setting.ini", QSettings::IniFormat);

    timer = new QTimer();
    autoTimer = new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    connect(autoTimer, SIGNAL(timeout()), this, SLOT(onAutoTimeout()));

    initChartsView();
    loadSettings();

    pointMaps["cpc"] = cpcPt1;
    pointMaps["cpc10"] = cpcPt10;
    simuPage = new ClientSimuPage();
    settingWindow = new ClientSettingWindow();
    stPage = new CpcStatusPage(dev);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(CalibClient *client)
{
    cpcClient = client;
    connect(cpcClient, SIGNAL(sigConnected()), this, SLOT(onConnected()));
    connect(cpcClient, SIGNAL(sigDisConnected()), this, SLOT(onDisconnected()));
    connect(cpcClient, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
    connect(cpcClient, SIGNAL(sigSetRet(QString,QString,QMap<QString,QString>))
            ,this, SLOT(onClientRet(QString,QString, QMap<QString,QString>)));
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
    if(settings->contains(QString("%1_upper").arg(deviceName)))
        ui->upperValue->setText(settings->value(QString("%1_upper").arg(deviceName)).toString());
    if(settings->contains(QString("%1_lower").arg(deviceName)))
        ui->lowerValue->setText(settings->value(QString("%1_lower").arg(deviceName)).toString());
    if(settings->contains(QString("%1_cpcaddr").arg(deviceName)))
        ui->cpcAddr->setText(settings->value(QString("%1_cpcaddr").arg(deviceName)).toString());
    if(settings->contains(QString("%1_port").arg(deviceName)))
        ui->listenPort->setText(settings->value(QString("%1_port").arg(deviceName)).toString());
    if(settings->contains(QString("%1_diameter").arg(deviceName)))
        ui->setMeter->setText(settings->value(QString("%1_diameter").arg(deviceName)).toString());
    if(settings->contains(QString("%1_avtime").arg(deviceName)))
        ui->averTime->setText(settings->value(QString("%1_avtime").arg(deviceName)).toString());
}

void MainWindow::onAutoTimeout()
{
    QList<QString> channels;
    channels << "27";
    cpcClient->getValue(channels);
}

void MainWindow::onTimerTimeout()
{
    qDebug()<<"request cpc data";
    QList<QString> getChannels;
    getChannels << "25";
//    connList[addrList["cpc"]]->getValue(getChannels);
//    if(connList.contains(addrList["test"]))
        cpcClient->getValue(getChannels);
    getChannels.clear();
    getChannels << "21" << "26" << "27" << "28";
//    connList[addrList["smps"]]->getValue(getChannels);
}

void MainWindow::initChartsView()
{
    cpc1slineSeries = new QLineSeries();
    cpc10slineSeries  = new QLineSeries();
//    testDevLineSeries = new QLineSeries();

    upperCalib = new QLineSeries();
    lowerCalib = new QLineSeries();
    upperCalib->setPen(QPen(QBrush(Qt::red), 2, Qt::DashLine));
    lowerCalib->setPen(QPen(QBrush(Qt::green), 2, Qt::DashLine));

    cpc1slineSeries->setUseOpenGL(true);
    cpc10slineSeries->setUseOpenGL(true);
//    testDevLineSeries->setUseOpenGL(true);

    cpc1slineSeries->setName("CPC(1s)");
    cpc10slineSeries->setName("CPC(10s)");
//    testDevLineSeries->setName("testDev");

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
//    m_chart->addSeries(testDevLineSeries);
    m_chart->addSeries(upperCalib);
    m_chart->addSeries(lowerCalib);

    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);

    cpc1slineSeries->attachAxis(yAxis);
    cpc1slineSeries->attachAxis(xAxis);

    cpc10slineSeries->attachAxis(yAxis);
    cpc10slineSeries->attachAxis(xAxis);

//    testDevLineSeries->attachAxis(xAxis);
//    testDevLineSeries->attachAxis(yAxis);

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
//    cpcTest.clear();
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
//    testDevLineSeries->clear();
    autom = true;
    calib = false;
    timer->stop();
    autoTimer->start(1000);
}

void MainWindow::on_menuBtn_clicked()
{
    stPage->show();
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
//    qDebug()<<connList.size();
//    qDebug()<<connList.keys();

//    if(connList.contains(addrList["cpc"]))
//    {
        cpcClient->enterClassifierMode(ui->setMeter->text());
//    }
//    if(connList.contains(addrList["smps"]))
//    {
//        connList[addrList["smps"]]->enterClassifierMode(ui->setMeter->text());
//    }
//    if(connList.contains(addrList["test"]))
//    {
//        connList[addrList["test"]]->enterClassifierMode(ui->setMeter->text());
//    }
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
    settings->setValue(QString("%1_upper").arg(deviceName), v);

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
    settings->setValue(QString("%1_lower").arg(deviceName), v);
}

void MainWindow::on_averTime_textChanged(const QString &arg1)
{
    bool ret;
    if(arg1.toInt(&ret))
        settings->setValue(QString("%1_avtime").arg(deviceName), arg1);
}

void MainWindow::on_saveBtn_clicked()
{
    if(!checkIp(ui->cpcAddr->text()))
    {
        QMessageBox::warning(this, "提示", tr("invalid ip address"), QMessageBox::Ok);
        return;
    }

    settings->setValue(QString("%1_cpcaddr").arg(deviceName), ui->cpcAddr->text());
    int port = ui->listenPort->text().toInt();
    if(port < 0 || port > 65535)
    {
        QMessageBox::warning(this, "提示", tr("invalid listen port"), QMessageBox::Ok);
        return;
    }
    settings->setValue(QString("%1_port").arg(deviceName), ui->listenPort->text());

}


void MainWindow::onClientData(QString client, QMap<QString,QString> data)
{
    qDebug()<<"onData" << client << data;
    if(client == "smps")
    {
//        ui->smpsVal21->setText(data["21"]);
//        ui->smpsVal26->setText(data["26"]);
//        ui->smpsVal27->setText(data["27"]);
//        ui->smpsVal28->setText(data["28"]);
    }

    if(1)
    {
        if(data.contains("25") && data.size() == 1)
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
        if(data.contains("25") && data.size() == 1)
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
//                if(cpcTest.size() < 600 )
//                {
//                    cpcTest.prepend(value);
//                }
//                else
//                {
//                    cpcTest.prepend(value);
//                    cpcTest.removeAt(cpcTest.size());
//                }

//                testDevLineSeries->clear();
//                for(int i = 0; i < cpcTest.size(); i ++)
//                {
//                    testDevLineSeries->append(i, cpcTest[i]);
//                }
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
            timer->start(1000);
//            enterClsDevs.append(type);
    }

//    if(enterClsDevs.contains("cpc") && enterClsDevs.contains("smps") && enterClsDevs.contains("test"))
//    if(enterClsDevs.contains("cpc") && enterClsDevs.contains("smps"))
//    {
//        enterClsDevs.clear();
//    }

//    if(enterAutoDevs.contains("cpc") && enterAutoDevs.contains("smps") && enterAutoDevs.contains("test"))
//    {
//        enterAutoDevs.clear();
//    }
}

void MainWindow::on_clientListBtn_clicked()
{
    simuPage->show();
}

void MainWindow::on_setMeter_textChanged(const QString &arg1)
{
//    qDebug()<<"text changed";
    bool ret;
    ret = arg1.toInt(&ret);
    if(ret)
        settings->setValue(QString("%1_diameter").arg(deviceName), arg1);
}

void MainWindow::on_tableBtn_clicked()
{
}

void MainWindow::on_cpcConnBtn_clicked()
{
    cpcClient->connectToHost(ui->cpcAddr->text(),  ui->listenPort->text().toUInt());
}

void MainWindow::onConnected()
{
    qDebug()<< "on connected";
}

void MainWindow::onDisconnected()
{
    qDebug()<< "on disconnect";
}
