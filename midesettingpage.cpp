#include "midesettingpage.h"
#include "ui_midesettingpage.h"

MideSettingPage::MideSettingPage(QString dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MideSettingPage)
{
    ui->setupUi(this);
    setWindowTitle(dev);
    devName = dev;

    setting = new QSettings("setting.ini", QSettings::IniFormat);
    if(setting->contains(QString("%1ip").arg(devName)))
    {
        ui->ip->setText(setting->value(QString("%1ip").arg(devName)).toString());
        ui->port->setText(setting->value(QString("%1port").arg(devName)).toString());
    }
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

void MideSettingPage::on_setTempBtn_clicked()
{

}

void MideSettingPage::saveParam()
{
    setting->setValue(QString("%1ip").arg(devName), ui->ip->text());
    setting->setValue(QString("%1port").arg(devName), ui->port->text());
}
void MideSettingPage::on_saveBtn_clicked()
{
    saveParam();
}
