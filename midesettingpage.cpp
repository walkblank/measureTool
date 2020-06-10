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
}

MideSettingPage::~MideSettingPage()
{
    delete ui;
}

void MideSettingPage::setClient(Md19Client *cli)
{
    client = cli;
    connect(client, SIGNAL(connected()), this, SLOT(onClientConnected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onClienDisconnected()));
    connect(client, SIGNAL(sigData(int, QVariant, QVariant)), this, SLOT(onSigData(int, QVariant, QVariant)));
}

void MideSettingPage::onClientConnected()
{
    qDebug()<< "page connected";
    ui->ip->setStyleSheet("background-color: rgb(0, 255, 127);");
    saveParam();
    ui->connBtn->setText("断开连接");
}

void MideSettingPage::onClienDisconnected()
{
    qDebug()<< "page disconnected";
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

void MideSettingPage::onSigData(int cmd, QVariant var, QVariant var1)
{
    if(cmd == 0x4)
    {
        ui->curTemp->setText(var.toString());
        ui->lllValue->setText(var1.toString());
    }
}
void MideSettingPage::on_setTempBtn_clicked()
{
    if(client->state() != QAbstractSocket::ConnectedState)
        QMessageBox::warning(this, "提示", "请先连接设备", QMessageBox::Ok);
    char temp = ui->tempSel->currentData().toInt();
    char xishiv;
    double factor = 1;
    QString curTempStr = ui->tempSel->currentText();
    if(curTempStr == "120")
        factor = 1.11;
    else if(curTempStr == "150")
        factor = 1.21;
    xishiv = 150/ui->xsValue->text().toUInt()*factor;

    emit sigXishiVal(ui->xsValue->text());
    client->setTemp(temp, xishiv);
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
}
