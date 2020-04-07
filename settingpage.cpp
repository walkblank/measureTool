#include "settingpage.h"
#include "ui_settingpage.h"

SettingPage::SettingPage(QMap<QString,CalibClient*> &map, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingPage), clientList(map)
{
    ui->setupUi(this);
}

SettingPage::~SettingPage()
{
    delete ui;
}

void SettingPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    setFixedSize(size().width(), size().height());
}

void SettingPage::onClientData(QString type , QMap<QString,QString> values)
{
    qDebug()<<"type" << type;
    qDebug()<<"values" << values;
    if(type == "smps" && values.size() > 1)
    {
        ui->smpsVal0->setText(values["0"]);
        ui->smpsVal1->setText(values["1"]);
        ui->smpsVal2->setText(values["2"]);
        ui->smpsVal3->setText(values["3"]);
        ui->smpsVal4->setText(values["4"]);
        ui->smpsVal5->setText(values["5"]);
        ui->smpsVal8->setText(values["8"]);
        ui->smpsVal9->setText(values["9"]);
        ui->smpsVal21->setText(values["21"]);
        ui->smpsVal22->setText(values["22"]);
        ui->smpsVal25->setText(values["25"]);
    }
    else if(type == "cpc" && values.size() > 1)
    {
        //        ui->cpcVal0->setText(values["0"]);
        if(values["0"] == "0")
        {
            ui->cpcVal0->setStyleSheet("background-color: rgb(255, 255, 0);");
        }
        else
            ui->cpcVal0->setStyleSheet("background-color: rgb(85, 255, 0);");
        ui->cpcVal4->setText(values["4"]);
        ui->cpcVal24->setText(values["24"]);
        ui->cpcVal27->setText(values["27"]);
        ui->cpcVal29->setText(values["29"]);
        ui->cpcVal30->setText(values["30"]);
    }
}

void SettingPage::on_readSMPSBtn_clicked()
{
    foreach(CalibClient *client, clientList.values())
    {
        if(client->getClientType() == "smps")
        {
            QList<QString> getVals;
            getVals << "0" << "1" << "2" << "3" << "4" << "5" << "8" << "9" << "21" << "22" << "25";
            client->getValue(getVals);
        }
    }
}

void SettingPage::on_readCPCBtn_clicked()
{
    foreach(CalibClient *client, clientList.values())
    {
        if(client->getClientType() == "cpc")
        {
            QList<QString> getVals;
            getVals << "0" << "24" << "27" << "29" << "30" <<"4";
            client->getValue(getVals);
        }
    }
}
