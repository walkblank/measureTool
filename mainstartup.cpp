#include "mainstartup.h"
#include "ui_mainstartup.h"

#include <QMessageBox>

MainStartUp::MainStartUp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainStartUp)
{
    ui->setupUi(this);
    setFixedSize(width(), height());

    cpc1Client = new CalibClient();
    cpc2Client = new CalibClient();
    smpsClient = new CalibClient();

    cpc1Window = new MainWindow("CPC1");
    cpc1Window->setClient(cpc1Client);
    cpc1Window->setSmpsClient(smpsClient);

    cpc2Window = new MainWindow("CPC2");
    cpc2Window->setClient(cpc2Client);
    cpc2Window->setSmpsClient(smpsClient);

    connect(cpc1Client, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onCpcDataSig(QString, QMap<QString,QString>)));

    connect(cpc2Client, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onCpcDataSig(QString, QMap<QString,QString>)));

    smpsPage = new SmpsStatusPage();
    smpsPage->setClient(smpsClient);

    connect(cpc2Client, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            smpsPage, SLOT(onClientData(QString, QMap<QString,QString>)));

    mdClient1 = new Md19Client();
    mdClient2 = new Md19Client();
    connect(mdClient1, SIGNAL(connected()), this, SLOT(onMdClientConnect()));
    connect(mdClient2, SIGNAL(connected()), this, SLOT(onMdClientConnect()));

    mide1Page = new MideSettingPage("MD19_3E-1");
    mide1Page->setClient(mdClient1);
    mide2Page = new MideSettingPage("MD19_3E-2");
    mide2Page->setClient(mdClient2);

    connect(mdClient1, SIGNAL(sigData(int , QVariant, QVariant)), SLOT(onSigData(int, QVariant, QVariant)));
    connect(mdClient2, SIGNAL(sigData(int , QVariant, QVariant)), SLOT(onSigData(int, QVariant, QVariant)));

    connect(mide1Page, SIGNAL(sigXishiVal(QString, QString)), this, SLOT(onXishiValSig(QString, QString)));
    connect(mide2Page, SIGNAL(sigXishiVal(QString, QString)), this, SLOT(onXishiValSig(QString, QString)));

    connect(mide1Page, SIGNAL(sigXishiVal(QString, QString)), cpc1Window, SLOT(onXishiSig(QString, QString)));
    connect(mide1Page, SIGNAL(sigXishiVal(QString, QString)), cpc2Window, SLOT(onXishiSig(QString, QString)));

    connect(mide2Page, SIGNAL(sigXishiVal(QString, QString)), cpc1Window, SLOT(onXishiSig(QString, QString)));
    connect(mide2Page, SIGNAL(sigXishiVal(QString, QString)), cpc2Window, SLOT(onXishiSig(QString, QString)));


    mdStartBtns << ui->startMd1Btn << ui->startMd2Btn;

    server = new MeasureServer(3320);
    server1 = new MeasureServer(3310);
    server2 = new MeasureServer(3330);
}

void MainStartUp::onMdClientConnect()
{
    Md19Client *client = static_cast<Md19Client*>(sender());
    if(client == mdClient1)
        qDebug()<<"client1";
    qDebug()<<"connect";
}
void MainStartUp::showEvent(QShowEvent *evt)
{
   Q_UNUSED(evt)
}

MainStartUp::~MainStartUp()
{
    delete ui;
}

void MainStartUp::on_openMd1Btn_clicked()
{
    if(mide1Page->isVisible())
        mide1Page->raise();
    else
        mide1Page->show();
}

void MainStartUp::on_openMd2Btn_clicked()
{
    if(mide2Page->isVisible())
        mide2Page->raise();
    else
        mide2Page->show();
}

void MainStartUp::on_openCpc1Btn_clicked()
{
    if(cpc1Window->isVisible())
        cpc1Window->raise();
    else
        cpc1Window->show();
}

void MainStartUp::on_openCpc2Btn_clicked()
{
    if(cpc2Window->isVisible())
        cpc2Window->raise();
    else
        cpc2Window->show();
}

void MainStartUp::on_openSmpsBtn_clicked()
{
    if(smpsPage->isVisible())
        smpsPage->raise();
    else
        smpsPage->show();
}

void MainStartUp::on_startMd1Btn_clicked()
{
    if(mdClient1->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this, "提示", "请先连接设备", QMessageBox::Ok);
        return;
    }
    if(mdClient1->getStart())
        mdClient1->stopDev();
    else
        mdClient1->startDev();
}

void MainStartUp::on_startMd2Btn_clicked()
{
    if(mdClient2->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this, "提示", "请先连接设备", QMessageBox::Ok);
        return;
    }
    if(mdClient2->getStart())
        mdClient2->stopDev();
    else
        mdClient2->startDev();
}

void MainStartUp::onXishiValSig(QString val, QString devName)
{
    MideSettingPage *page = static_cast<MideSettingPage *>(sender());
    if(page == mide1Page)
        ui->md1DilutionVal->setText(val);
    else
        ui->md2DilutionVal->setText(val);
}

void MainStartUp::onCpcDataSig(QString type, QMap<QString,QString> values)
{
    if(values.contains("28"))
    {
        CalibClient *client = static_cast<CalibClient*>(sender());
        if(client == cpc1Client)
        {
            ui->cpc1Cn->setText(values["27"]);
            ui->cpc1FlowRate->setText(values["28"]);
        }
        else
        {
            ui->cpc2Cn->setText(values["27"]);
            ui->cpc2FlowRate->setText(values["28"]);
        }
    }
}

void MainStartUp::onSigData(int cmd, QVariant var, QVariant var1)
{
    Q_UNUSED(var1)
    int idx = 0;

    Md19Client *client = static_cast<Md19Client*>(sender());
    if(client == mdClient2)
        idx = 1;
    qDebug()<< "on sigData" << cmd;
    if(cmd == 0x5)
    {
        if(var.toBool())
            mdStartBtns[idx]->setText("泵 停止");
        else
            mdStartBtns[idx]->setText("泵 启动");
    }
}
