#include "testdevpage.h"
#include "ui_testdevpage.h"

TestDevPage::TestDevPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestDevPage)
{
    ui->setupUi(this);

    setWindowTitle("测试设备");
    settings = new QSettings("setting.ini", QSettings::IniFormat);
    if(settings->contains("testAddr"))
        ui->ipAddr->setText(settings->value("testAddr").toString());
    if(settings->contains("testPort"))
        ui->port->setText(settings->value("testPort").toString());
    if(settings->contains("ak"))
        ui->chkAKDev->setChecked(settings->value("ak").toBool());
    if(settings->contains("cpc"))
        ui->chkAKDev->setChecked(settings->value("cpc").toBool());
}

void TestDevPage::setTestClient(CalibClient *client)
{
    testClient = client;
    connect(testClient, SIGNAL(sigConnected()), this, SLOT(onConnected()));
    connect(testClient, SIGNAL(sigDisConnected()), this, SLOT(onDisconnected()));
    connect(testClient, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
}

void TestDevPage::onClientData(QString devName, QMap<QString, QString> data)
{

}

void TestDevPage::saveParam()
{
    settings->setValue("testAddr", ui->ipAddr->text());
    settings->setValue("testPort", ui->port->text());
    settings->setValue("ak", ui->chkAKDev->isChecked());
    settings->setValue("cpc", ui->chkCPCDev->isChecked());
}

void TestDevPage::onConnected()
{
    ui->ipAddr->setStyleSheet("background-color: rgb(0, 255, 127);");
    ui->connBtn->setText("断开");
    saveParam();
    ui->chkAKDev->setDisabled(true);
    ui->chkCPCDev->setDisabled(true);
}

void TestDevPage::onDisconnected()
{
    ui->ipAddr->setStyleSheet("background-color: rgb(255, 255, 127);");
    ui->connBtn->setText("连接");
    ui->chkAKDev->setDisabled(false);
    ui->chkCPCDev->setDisabled(false);
}

TestDevPage::~TestDevPage()
{
    delete ui;
}

void TestDevPage::on_connBtn_clicked()
{
    qDebug()<<"smsp conn";
    if(testClient->state() != QAbstractSocket::ConnectedState)
    {
        if(ui->chkAKDev->isChecked())
            testClient->setClientType("ak");
        testClient->connectToHost(ui->ipAddr->text(),  ui->port->text().toUInt());
    }
    else
    {
        testClient->disconnect();
        testClient->setClientType(QString());
    }
}
