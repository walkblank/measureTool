#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QHostInfo>
#include <QDateTime>

//TODO
// 1 stop timer when disconnect from cpc devcie


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
    xAxis = new QValueAxis;
    initChartsView();
    loadSettings();

    pointMaps["cpc"] = cpcPt1;
    pointMaps["cpc10"] = cpcPt10;
    simuPage = new ClientSimuPage();
    settingWindow = new ClientSettingWindow();
    stPage = new CpcStatusPage(dev);
    ui->clientListBtn->hide();

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
            this, SLOT(onClientData1(QString, QMap<QString,QString>)));
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
    channels << "27" << "28" << "12";
    cpcClient->getValue(channels);
}

void MainWindow::onTimerTimeout()
{
    qDebug()<<"request cpc data";
    QList<QString> getChannels;
    getChannels << "27" << "28" << "12";
    cpcClient->getValue(getChannels);
}

void MainWindow::initChartsView()
{
    cpc1slineSeries = new QLineSeries();
    cpc10slineSeries  = new QLineSeries();
    testlineSeries = new QLineSeries();

    upperCalib = new QLineSeries();
    lowerCalib = new QLineSeries();
    upperCalib->setPen(QPen(QBrush(Qt::red), 2, Qt::DashLine));
    lowerCalib->setPen(QPen(QBrush(Qt::green), 2, Qt::DashLine));

    cpc1slineSeries->setUseOpenGL(true);
    cpc10slineSeries->setUseOpenGL(true);
    testlineSeries->setUseOpenGL(true);

    cpc1slineSeries->setName("CPC(1s)");
    cpc10slineSeries->setName("CPC(10s)");
    testlineSeries->setName("Test(1s)");

    upperCalib->setName("上限");
    lowerCalib->setName("下限");

    m_chartView = new QChartView(m_chart);
    m_chartView->setMinimumSize(1200, 600);

    xAxis->setRange(0, timeRange);
    xAxis->setTickCount(31);
    xAxis->setMinorTickCount(1);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText("time(s)");

//    QValueAxis *yAxis = new QValueAxis;
    //yAxis->setRange(-1, 1);
    yAxis->setRange(minYrange, maxYrange);
    ui->startVal->setText(QString("%1").arg(minYrange));
    ui->endVal->setText(QString("%1").arg(maxYrange));
    yAxis->setTickCount(11);
    yAxis->setLabelFormat("%.2f");
    yAxis->setMinorTickCount(1);
    yAxis->setTitleText("value");

    m_chart->addSeries(cpc1slineSeries);
    m_chart->addSeries(cpc10slineSeries);
    m_chart->addSeries(testlineSeries);
    m_chart->addSeries(upperCalib);
    m_chart->addSeries(lowerCalib);

    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);

    cpc1slineSeries->attachAxis(yAxis);
    cpc1slineSeries->attachAxis(xAxis);

    cpc10slineSeries->attachAxis(yAxis);
    cpc10slineSeries->attachAxis(xAxis);

    testlineSeries->attachAxis(yAxis);
    testlineSeries->attachAxis(xAxis);

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


    if(autoTimer->isActive())
    {
        ui->startBtn->setText("开始取值");
        autoTimer->stop();

        QXlsx::Document  xlsx;
        xlsx.write(1, 1, "日期");
        xlsx.write(1, 2, "时间");
        xlsx.write(1, 3, "Cn");
        xlsx.write(1, 4, "流量");
        xlsx.write(1, 5, "压力");
        xlsx.write(1, 6, "日期(test)");
        xlsx.write(1, 7, "时间(test)");
        if(testDevType == "ak")
        {
            for(int i = 3; i < 24; i++)
                xlsx.write(1, i+5, QString("%1(testAK)").arg(i));
        }
        else
        {
            xlsx.write(1, 8, "Cn(test)");
            xlsx.write(1, 9, "流量(test)");
            xlsx.write(1, 10, "压力(test)");
            xlsx.write(1, 11, "MD19-3E-1");
            xlsx.write(1, 12, "MD19-3E-2");
        }

        int index = 1;
        foreach(QString flow,  cpcFlowList)
        {
            xlsx.write(index+1, 1, datetimeList.at(index-1).split('-')[0]);
            xlsx.write(index+1, 2, datetimeList.at(index-1).split('-')[1]);
            xlsx.write(index+1, 3, QString("%1").arg(cpcVList.at(index-1)));
            xlsx.write(index+1, 4, cpcFlowList.at(index-1));
            xlsx.write(index+1, 5, cpcPressureList.at(index-1));

            xlsx.write(index+1, 11, xishiVals["MD19_3E-1"]) ;
            xlsx.write(index+1, 12, xishiVals["MD19_3E-2"]);
            index+=1;
        }
        index = 1;
        foreach(QString flow,  testFlowList)
        {
            xlsx.write(index+1, 6, testDatetimeList.at(index-1).split('-')[0]);
            xlsx.write(index+1, 7, testDatetimeList.at(index-1).split('-')[1]);
            if(testDevType == "ak")
            {
                for(int i = 3; i < 24; i++)
                    xlsx.write(index+1, i+5, akTestDataList.at(index-1)[QString("%1").arg(i)]);
            }
            else
            {
                xlsx.write(index+1, 8, QString("%1").arg(testVList.at(index-1)));
                xlsx.write(index+1, 9, testFlowList.at(index-1));
                xlsx.write(index+1, 10, testPresssureList.at(index-1));
            }
            index+=1;
        }

        xlsx.saveAs(QApplication::applicationDirPath().append(QString("/%1/%2.xlsx").arg(deviceName)
                                                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"))));
    }

    pointCnt = 0;
    cpcPt1.clear();
    cpcPt10.clear();
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
    testlineSeries->clear();
    tmpPt.clear();
    datetimeList.clear();
    tmpTestPt.clear();
    testDatetimeList.clear();
    cpcVList.clear();
    testVList.clear();
    cpcFlowList.clear();
    testFlowList.clear();
    cpcPressureList.clear();
    testPresssureList.clear();
    akTestDataList.clear();

    sampleCnt = ui->sampleCnt->text().toUInt();
    sampleInterval = ui->sampleInterval->text().toUInt();
    autom = true;
    calib = false;
    timer->stop();
    cpcClient->enterAutoMode();
    waitSomeTime(2000);
    //    if(smpsCient->getWorkingMode() == QString() || smpsCient->getWorkingMode() == "classifier")
    //        smpsCient->enterAutoMode();
    ui->startBtn->setText("停止取值");
}

void MainWindow::on_menuBtn_clicked()
{
    stPage->show();
}

void MainWindow::on_startCalibBtn_clicked()
{
    //clear all lines;
    pointCnt = 0;
    cpc1slineSeries->clear();
    cpc10slineSeries->clear();
    averageTime = ui->averTime->text().toInt();

    if(timer->isActive())
        timer->stop();
    calib = true;
    cpcClient->enterClassifierMode(ui->setMeter->text());
    waitSomeTime(2000);
    if(smpsCient->getWorkingMode() == QString())
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


void MainWindow::onTestData(QString client, QMap<QString, QString> data)
{
    double value;
    QString flowRate, pressure;
    testDevType = client;
//    autom = true;
    if(autom)
    {
        if(client == "ak")
        {
            value = data["3"].toDouble();
            akTestDataList.append(data);
//            qDebug()<<data;
//            flowRate = data["flowRate"];
//            pressure = data["pressure"];
        }
        else if(data.contains("28") && data.size() == 3)
        {
            value = data["28"].toDouble();
            flowRate = data["27"];
            pressure = data["12"];
            testFlowList.append(flowRate);
            testPresssureList.append(pressure);
        }

        testlineSeries->append(pointCnt, value);
        tmpTestPt.append(value);
        if(tmpTestPt.size() == sampleInterval)
        {
            double avalue = 0;
            foreach(double v, tmpTestPt)
                avalue += v;
            avalue = avalue/sampleInterval;
            testVList.append(avalue);
            testDatetimeList.append(QDateTime::currentDateTime().toString("yyyy/MM/dd-hh:mm:ss"));
            tmpTestPt.clear();
        }
    }
}

void MainWindow::onClientData1(QString client, QMap<QString, QString> data)
{
    static qint64 curMinValue = 0;
    if(data.contains("28") && data.size() == 3)
    {
        double value = data["28"].toDouble();
        QString flowRate = data["27"];
        QString pressure = data["12"];
        ui->cnValue->setText(data["28"]);

        if(autom)
        {
            tmpSavePt.append(value);
            if(tmpSavePt.size() == sampleInterval)
            {
                double avalue = 0;
                foreach(double v, tmpSavePt)
                    avalue += v;
                avalue = avalue/sampleInterval;
                cpcVList.append(avalue);
                cpcFlowList.append(flowRate);
                cpcPressureList.append(pressure);
                datetimeList.append(QDateTime::currentDateTime().toString("yyyy/MM/dd-hh:mm:ss"));
                tmpSavePt.clear();
            }
        }

        if(ui->autoAdjustYRangeChk->isChecked())
        {
            if(value > maxYrange)
            {
                maxYrange = value+value/2;
                ui->endVal->setText(QString("%1").arg(maxYrange));
            }
            if(value > minYrange)
            {
                if((value-value/2) > minYrange)
                {
                    minYrange = value - value/2;
                    curMinValue = value;
                }
                ui->startVal->setText(QString("%1").arg(minYrange));
            }
            else if (value < minYrange)
            {
                minYrange = value - value/2;
                if(minYrange < 0)
                    minYrange = 0;
                ui->startVal->setText(QString("%1").arg(minYrange));
            }

            yAxis->setRange(minYrange, maxYrange);
        }

        tmpPt.append(value);
        if(cpc1slineSeries->count() == 0)
            ui->startTime->setText(QDateTime::currentDateTime().toString("yyyy/dd/MM-hh:mm:ss"));
        ui->endTime->setText(QDateTime::currentDateTime().toString("yyyy/dd/MM-hh:mm:ss"));
        cpc1slineSeries->append(pointCnt, value);

        if(cpc1slineSeries->count() > timeRange)
        {
            timeRange = timeRange*2;
            xAxis->setRange(0, timeRange);
        }

        if(!autom)
        {
            double avVal = 10;
            if(tmpPt.size() == averageTime)
            {
                double sum = 0;
                foreach(double v, tmpPt)
                    sum += v;
                avVal = sum /averageTime;

                cpc10slineSeries->append(cpc1slineSeries->count(),  avVal);
                tmpPt.clear();

                if(avVal > ui->lowerValue->text().toDouble() && avVal < ui->upperValue->text().toDouble())
                    ui->startBtn->setDisabled(false);
            }
        }
        pointCnt += 1;
    }
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
            if(devName == "smps")
                smpsCient->setWorkingMode("classifier");
            if(ret == "ok")
                classReadyDevMap[devName] = true;
            if(classReadyDevMap.contains("cpc") && classReadyDevMap.contains("smps"))
            {
                timer->start(1000);
//                classReadyDevMap.clear();
                classReadyDevMap.remove("cpc");
            }
        }
        if(setValues["201"] == "1")
        {
//            if(devName == "smps")
//                smpsCient->setWorkingMode("auto");
            if(ret == "ok")
                autoReadyDevMap[devName] = true;
//            if(autoReadyDevMap.contains("cpc") && autoReadyDevMap.contains("smps"))
            if(autoReadyDevMap.contains("cpc"))
            {
                autoTimer->start(1000);
//                autoReadyDevMap.clear();
                emit sigStartQuery();
                autoReadyDevMap.remove("cpc");
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
//    on_startCalibBtn_clicked();
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

void MainWindow::on_meterSetBtn_clicked()
{
   cpcClient->setMeter(ui->setMeter->text());
}

void MainWindow::on_autoAdjustYRangeChk_toggled(bool checked)
{
   ui->startVal->setDisabled(checked);
   ui->endVal->setDisabled(checked);
   ui->setRangeBtn->setDisabled(checked);
}
