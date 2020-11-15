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
}

void TestDevPage::setTestClient(CalibClient *client)
{
    testClient = client;
    connect(testClient, SIGNAL(sigConnected()), this, SLOT(onConnected()));
    connect(testClient, SIGNAL(sigDisConnected()), this, SLOT(onDisconnected()));
    connect(testClient, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
}

void TestDevPage::saveParam()
{
    settings->setValue("testAddr", ui->ipAddr->text());
    settings->setValue("testPort", ui->port->text());
}

void TestDevPage::onConnected()
{
    ui->ipAddr->setStyleSheet("background-color: rgb(0, 255, 127);");
    ui->connBtn->setText("断开");
    saveParam();
}

void TestDevPage::onDisconnected()
{
    ui->ipAddr->setStyleSheet("background-color: rgb(255, 255, 127);");
    ui->connBtn->setText("连接");
}

TestDevPage::~TestDevPage()
{
    delete ui;
}

void TestDevPage::on_connBtn_clicked()
{
    qDebug()<<"smsp conn";
    if(testClient->state() != QAbstractSocket::ConnectedState)
        testClient->connectToHost(ui->ipAddr->text(),  ui->port->text().toUInt());
    else
        testClient->disconnect();
}
