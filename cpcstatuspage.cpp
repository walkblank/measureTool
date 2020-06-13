#include "cpcstatuspage.h"
#include "ui_cpcstatuspage.h"

CpcStatusPage::CpcStatusPage(QString dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpcStatusPage)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1设备状态").arg(dev));
    setFixedSize(width(), height());
    devName = dev;
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onCheckTimerTimeout()));
}

void CpcStatusPage::setClient(CalibClient *client)
{
    this->client = client;
    connect(client, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onDataReadSig(QString,QMap<QString,QString>)));
}

void CpcStatusPage::showEvent(QShowEvent *evt)
{
    Q_UNUSED(evt)
    qDebug()<<"cpc status page show";
    timer->start(2000);
}

void CpcStatusPage::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    qDebug()<<"cpc status page hide";
    timer->stop();
}

void CpcStatusPage::onCheckTimerTimeout()
{
    QList<QString> cmds;
    cmds << "29" << "24" << "30" << "0" << "27";

    client->getValue(cmds);
}

void CpcStatusPage::onDataReadSig(QString type, QMap<QString,QString> values)
{
    Q_UNUSED(type)
    if(values.keys().contains("30"))
    {
        if(values["0"] == "0")
        {
            ui->val0->setStyleSheet("background-color: rgb(255, 255, 0);");
        }
        else
            ui->val0->setStyleSheet("background-color: rgb(85, 255, 0);");
        ui->val24->setText(values["24"]);
        ui->val27->setText(values["27"]);
        ui->val29->setText(values["29"]);
        ui->val30->setText(values["30"]);
    }
}
CpcStatusPage::~CpcStatusPage()
{
    delete ui;
}

void CpcStatusPage::on_setVal127Btn_clicked()
{
    QMap<QString,QString> cmdMap;
    cmdMap["127"] = ui->valset127->text();

    client->setValue(cmdMap);
}

void CpcStatusPage::on_setVal128Btn_clicked()
{
    QMap<QString,QString> cmdMap;
    cmdMap["128"] = ui->valset128->text();

    client->setValue(cmdMap);
}

void CpcStatusPage::on_readValBtn_clicked()
{
    QList<QString> cmds;
    cmds << "29" << "24" << "30" << "0" << "27" << "127" << "128";
    client->getValue(cmds);
}
