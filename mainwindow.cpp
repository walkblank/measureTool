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

    yAxis = new QValueAxis;
    initChartsView();
    loadSettings();

    pointMaps["cpc"] = cpcPt1;
    pointMaps["cpc10"] = cpcPt10;
    simuPage = new ClientSimuPage();
    settingWindow = new ClientSettingWindow();
    stPage = new CpcStatusPage(dev);
    ui->clientListBtn->hide();
    ui->startCalibBtn->hide();

    QString dirPath = QApplication::applicationDirPath().append(QString("/%1").arg(dev));
    QFileInfo finfo(dirPath);
    if(!finfo.exists())
    {
       QDir dir;
       dir.mkdir(dirPath);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(CalibClient *client)
{
    cpcClient = client;
    stPage->setClient(cpcClient);
    connect(cpcClient, SIGNAL(sigConnected()), this, SLOT(onConnected()));
    connect(cpcClient, SIGNAL(sigDisConnected()), this, SLOT(onDisconnected()));

    connect(cpcClient, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
    connect(cpcClient, SIGNAL(sigSetRet(QString,QString,QMap<QString,QString>))
            ,this, SLOT(onClientRet(QString,QString, QMap<QString,QString>)));
}

void MainWindow::setSmpsClient(CalibClient *client)
{
    smpsCient = client;

    connect(smpsCient, SIGNAL(sigSetRet(QString,QString,QMap<QString,QString>))
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

void MainWindow::waitSomeTime(int ms)
{
    QTime time;
    time.start();
    while(time.elapsed() <  ms)
        qApp->processEvents();
}

void MainWindow::xlsxFileWriteHeader(QXlsx::Document *file)
{
    if(file == nullptr)
        return;
    file->write(1, 1, "时间");
    file->write(1, 2, "Cn");
    file->write(1, 3, "流量");
    file->write(1, 4, "MD19-3E-1");
    file->write(1, 5, "MD19-3E-2");
}

void MainWindow::xlsxFilwWriteRecord(QXlsx::Document *file, int row,  QString time, QString count, QString flowRate, QString md1, QString md2)
{
    if(file==nullptr)
        return;

    file->write(row+2, 1, time);
    file->write(row+2, 2, count);
    file->write(row+2, 3, flowRate);
    file->write(row+2, 4, md1);
    file->write(row+2, 5, md2);
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
    if(settings->contains(QString("%1_sampleInterval").arg(deviceName)))
        ui->sampleInterval->setText(settings->value(QString("%1_sampleInterval").arg(deviceName)).toString());
    if(settings->contains(QString("%1_sampleCnt").arg(deviceName)))
        ui->sampleCnt->setText(settings->value(QString("%1_sampleCnt").arg(deviceName)).toString());
}

void MainWindow::onAutoTimeout()
{
    QList<QString> channels;
    channels << "27" << "28";
    cpcClient->getValue(channels);
}

void MainWindow::onTimerTimeout()
{
    qDebug()<<"request cpc data";
    QList<QString> getChannels;
    getChannels << "27" << "28";
    cpcClient->getValue(getChannels);
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
    m_chartView->setMinimumSize(1200, 600);

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 600);
    xAxis->setTickCount(31);
    xAxis->setMinorTickCount(1);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText("time(s)");

//    QValueAxis *yAxis = new QValueAxis;
    //yAxis->setRange(-1, 1);
    yAxis->setRange(0, 50000);
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
    //if(xishiVals.size() != 2)
    //{
      //  QMessageBox::warning(this, "提示", "请先设置稀释值", QMessageBox::Ok);
        //return;
    //}

    currXlsx = new QXlsx::Document(QString("%1/%3/%2.xlsx").arg(QApplication::applicationDirPath())
                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"))
                                   .arg(deviceName));
    xlsxFileWriteHeader(currXlsx);

    cpcPt1.clear();
    cpcPt10.clear();
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();

    sampleCnt = ui->sampleCnt->text().toUInt();
    sampleInterval = ui->sampleInterval->text().toUInt();
    autom = true;
    calib = false;
    timer->stop();
    cpcClient->enterAutoMode();
    waitSomeTime(2000);
    smpsCient->enterAutoMode();
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
    cpcClient->enterClassifierMode(ui->setMeter->text());
    waitSomeTime(2000);
    smpsCient->enterSmpsClassifierMode(ui->setMeter->text());
}

void MainWindow::onXishiSig(QString val, QString devName)
{
    xishiVals[devName] = val;
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
    qDebug()<< deviceName <<"onData" << client << data;

    if(data.contains("28") && data.size() == 2)
    {
        double value = data["28"].toDouble();
        QString flowRate = data["27"];
        ui->cnValue->setText(data["28"]);

        if(autom)
        {
            tmpSavePt.append(value);
            datetimeList.append(QDateTime::currentDateTime().toString("yyyy-MMdd-hh:mm:ss"));
            cpcVList.append(value);
            if(cpcVList.size() >= testVList.size() && errVList.size() < testVList.size())
            {
                errVList.append(cpcVList[errVList.size()] - testVList[errVList.size()]);
            }

            if(tmpSavePt.size() == sampleInterval)
            {
                double avalue = 0;
                foreach(double v, tmpSavePt)
                    avalue += v;
                avalue = avalue/sampleInterval;
                tmpSavePt.clear();

                xlsxFilwWriteRecord(currXlsx, writeCnt, QDateTime::currentDateTime().toString("yyyy/MM/dd-hh:mm:ss")
                                    , QString("%1").arg(avalue), flowRate, xishiVals["MD19_3E-1"], xishiVals["MD19_3E-2"]);
                writeCnt += 1;
            }
            if(writeCnt == sampleCnt)
            {
                autoTimer->stop();
                writeCnt = 0;
                currXlsx->save();
                delete currXlsx;

                QMessageBox::information(this, "提示", "采样结束，采样数据已保存", QMessageBox::Ok);
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

        double avVal = 10;
        if(tmpPt.size() == averageTime)
        {
            double sum = 0;
            foreach(double v, tmpPt)
                sum += v;
            avVal = sum/averageTime;
           qDebug()<<"averaage " << avVal << deviceName;
            if(cpcPt10.size() < 600/averageTime)
            {
                cpcPt10.prepend(avVal);
            }
            else
            {
                cpcPt10.prepend(avVal);
                cpcPt10.removeAt(cpcPt10.size());
            }
            tmpPt.clear();
            qDebug()<< cpcPt10;
        }

        cpc1slineSeries->clear();
        cpc10slineSeries->clear();
        for(int i = 0; i < cpcPt1.size(); i ++)
            cpc1slineSeries->append(i, cpcPt1[i]);

        if(!autom)
        {
            cpc10slineSeries->clear();
            for(int i = 0; i < cpcPt10.size(); i ++)
                cpc10slineSeries->append(i*averageTime+cpcPt1.size()-averageTime*cpcPt10.size(), cpcPt10[i]);

            if(cpcPt10.size() > 0 && avVal > ui->lowerValue->text().toDouble() && avVal < ui->upperValue->text().toDouble())
                ui->startBtn->setDisabled(false);
        }
    }
}

void MainWindow::onClientRet(QString type, QString ret, QMap<QString,QString> setValues)
{
    Q_UNUSED(type)
    QString devName;
    if(setValues.contains("201"))
    {
        CalibClient *client = static_cast<CalibClient*>(sender());
        client == cpcClient ?  devName = "cpc" : devName = "smps";

        mutex.lock();
        if(setValues["201"] == "4")
        {
            if(ret == "ok")
                classReadyDevMap[devName] = true;
            if(classReadyDevMap.contains("cpc") && classReadyDevMap.contains("smps"))
            {
                timer->start(1000);
                classReadyDevMap.clear();
            }
        }
        if(setValues["201"] == "1")
        {
            if(ret == "ok")
                autoReadyDevMap[devName] = true;
            if(autoReadyDevMap.contains("cpc") && autoReadyDevMap.contains("smps"))
            {
                autoTimer->start(1000);
                autoReadyDevMap.clear();
            }
        }
        mutex.unlock();
    }
}

void MainWindow::on_clientListBtn_clicked()
{
    simuPage->show();
}

void MainWindow::on_setMeter_textChanged(const QString &arg1)
{
    bool ret;
    arg1.toInt(&ret);
    if(ret)
        settings->setValue(QString("%1_diameter").arg(deviceName), arg1);
}

void MainWindow::on_tableBtn_clicked()
{
}

void MainWindow::on_cpcConnBtn_clicked()
{
    if(cpcClient->state() != QAbstractSocket::ConnectedState)
        cpcClient->connectToHost(ui->cpcAddr->text(),  ui->listenPort->text().toUInt());
    else
        cpcClient->disconnect();
}

void MainWindow::onConnected()
{
    ui->cpcAddr->setStyleSheet("background-color: rgb(0, 255, 127);");
    qDebug()<< "on connected";
    ui->cpcConnBtn->setText("断开连接");
    settings->setValue(QString("%1_cpcaddr").arg(deviceName), ui->cpcAddr->text());
    settings->setValue(QString("%1_port").arg(deviceName), ui->listenPort->text());
    if(smpsCient->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this, "提示", "SMPS设备尚未连接", QMessageBox::Ok);
        return;
    }
    on_startCalibBtn_clicked();
}

void MainWindow::onDisconnected()
{
    ui->cpcAddr->setStyleSheet("background-color: rgb(255, 255, 127);");
    qDebug()<< "on disconnect";
    ui->cpcConnBtn->setText("连接设备");
}

void MainWindow::on_sampleInterval_textChanged(const QString &arg1)
{
   bool ret;
   arg1.toUInt(&ret);
   if(ret)
       settings->setValue(QString("%1_sampleInterval").arg(deviceName), arg1);
}

void MainWindow::on_sampleCnt_textChanged(const QString &arg1)
{
   bool ret;
   arg1.toUInt(&ret);
   if(ret)
       settings->setValue(QString("%1_sampleCnt").arg(deviceName), arg1);
}

void MainWindow::on_setRangeBtn_clicked()
{
    yAxis->setRange(ui->startVal->text().toUInt(), ui->endVal->text().toUInt());
}
