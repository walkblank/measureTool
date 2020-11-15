#include "midesettingpage.h"
#include "ui_midesettingpage.h"

#include <QMessageBox>

MideSettingPage::MideSettingPage(QString dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MideSettingPage)
{
    ui->setupUi(this);
    setWindowTitle(dev);
    devName = dev;

    setting = new QSettings("setting.ini", QSettings::IniFormat);
    if(setting->contains(QString("%1_ip").arg(devName)))
    {
        ui->ip->setText(setting->value(QString("%1_ip").arg(devName)).toString());
        ui->port->setText(setting->value(QString("%1_port").arg(devName)).toString());
    }
    if(setting->contains(QString("%1_xsVal").arg(devName)))
        ui->xsValue->setText(setting->value(QString("%1_xsVal").arg(devName)).toString());

    ui->tempSel->addItem("80", 4);
    ui->tempSel->addItem("120", 6);
    ui->tempSel->addItem("150", 7);
    setFixedSize(width(), height());

    queryTimer = new QTimer();
    connect(queryTimer, SIGNAL(timeout()), this, SLOT(queryParam()));
}

MideSettingPage::~MideSettingPage()
{
    delete ui;
}

void MideSettingPage::showEvent(QShowEvent *evt)
{
    Q_UNUSED(evt)
    if(!queryTimer->isActive() && (client->state() == QAbstractSocket::ConnectedState))
        queryTimer->start(3000);
}

void MideSettingPage::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    queryTimer->stop();
}

void MideSettingPage::setClient(Md19Client *cli)
{
    client = cli;
    connect(client, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onClienDisconnected()));
    connect(client, SIGNAL(sigData(int, QVariant, QVariant, int)), this, SLOT(onSigData(int, QVariant, QVariant, int)));
    connect(client, SIGNAL(sigParam(int,QMap<QString,QString>)), this, SLOT(onParam(int,QMap<QString,QString>)));
}

void MideSettingPage::onClientConnected()
{
    ui->ip->setStyleSheet("background-color: rgb(0, 255, 127);");
    saveParam();
    ui->connBtn->setText("断开连接");
    if(!queryTimer->isActive())
        queryTimer->start(3000);
}

void MideSettingPage::onClienDisconnected()
{
    ui->ip->setStyleSheet("background-color: rgb(255, 255, 127);");
    ui->connBtn->setText("连接设备");
}

void MideSettingPage::on_connBtn_clicked()
{
    if(client->state() == QAbstractSocket::ConnectedState)
    {
        client->disconnectFromHost();
    }
    else
    {
        client->connectToHost(ui->ip->text(), ui->port->text().toUInt());
    }
}

void MideSettingPage::queryParam()
{
    client->readTemp();
    client->readParam();
}

void MideSettingPage::onParam(int funcCode, QMap<QString, QString> param)
{
    qDebug()<<"funcCode" << funcCode <<  param;
    if(funcCode == 0x4)
    {
        ui->speed->setText(QString("%1").arg(param["speed"].toUInt(nullptr, 16)));
        ui->airTemp->setText(QString("%1").arg(param["temp"].toUInt(nullptr, 16)));
        ui->ariPressure->setText(QString("%1").arg(param["pressure"].toUInt(nullptr, 16)));
    }
}

void MideSettingPage::onSigData(int cmd, QVariant var, QVariant var1, int reg)
{
    if(cmd == 0x4)
    {
        if(reg == 7)
        {
        //        float tFactor = ui->tempSel->currentText().toUInt()/200.00;
        unsigned short tempValue = var.toString().toInt(nullptr, 16);
        unsigned short llValue = var1.toString().toInt(nullptr, 16);
        qDebug() << "read tempV" << tempValue << tempValue/50000.00 *200;
        ui->curTemp->setText(QString("%1").arg(tempValue/50000.00*200));
//        ui->lllValue->setText(var1.toString());
        ui->lllValue->setText(QString("%1").arg(llValue/50000.00*5));
        }
    }
    else if(cmd == 0x5)
    {
        if(reg == 0x0)
            ui->devStatus->setText(var.toBool() ? "泵启动" : "泵停止");
        else if(reg == 0x1)
            ui->remoteSwiBtn->setText(var.toBool() ? "关闭远程开关" : "开启远程开关");
    }
}
void MideSettingPage::on_setTempBtn_clicked()
{
    if(client->state() != QAbstractSocket::ConnectedState)
        QMessageBox::warning(this, "提示", "请先连接设备", QMessageBox::Ok);
    unsigned short temp = ui->tempSel->currentText().toUInt();
    unsigned short xishiv;
    double factor = 1;
    QString curTempStr = ui->tempSel->currentText();
    if(curTempStr == "120")
        factor = 1.11;
    else if(curTempStr == "150")
        factor = 1.21;
    xishiv = ((150*50000/ui->xsValue->text().toUInt()*factor)/10);


    float tFactor = temp/200.00;
    unsigned short tempV = tFactor*50000;
//    qDebug()<<"xishiv" << QString("%1").arg(xishiv) << QString("%1").arg(tempV);
    emit sigXishiVal(ui->xsValue->text(), devName);
    client->setTemp(tempV, xishiv);
    saveParam();
}

void MideSettingPage::saveParam()
{
    setting->setValue(QString("%1_ip").arg(devName), ui->ip->text());
    setting->setValue(QString("%1_port").arg(devName), ui->port->text());
    setting->setValue(QString("%1_xsVal").arg(devName), ui->xsValue->text());
}
void MideSettingPage::on_saveBtn_clicked()
{
    saveParam();
}

void MideSettingPage::on_readTempBtn_clicked()
{
    if(client->state() != QAbstractSocket::ConnectedState)
        QMessageBox::warning(this, "提示", "请先连接设备", QMessageBox::Ok);
    client->readTemp();
    client->readParam();
}

void MideSettingPage::on_remoteSwiBtn_clicked()
{
    client->setRemoteSwitch(!client->getSwitch());
}
