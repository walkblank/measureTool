#include "clientsimupage.h"
#include "ui_clientsimupage.h"

ClientSimuPage::ClientSimuPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientSimuPage)
{
    ui->setupUi(this);
    cpc = new CalibClient("ss");
    smps= new CalibClient("ss");
    test = new CalibClient("ss");
    QSettings settings("setting.ini", QSettings::IniFormat);

    if(settings.contains("cpcaddr"))
        ui->cpcHost->setText(settings.value("cpcaddr").toString());
    if(settings.contains("smpsaddr"))
        ui->smpsHost->setText(settings.value("smpsaddr").toString());
    if(settings.contains("testaddr"))
        ui->testDevHost->setText(settings.value("testaddr").toString());
    ui->port->setText(settings.value("port").toString());
}

ClientSimuPage::~ClientSimuPage()
{
    delete ui;
}

void ClientSimuPage::on_cpcConnBtn_clicked()
{
    cpc->connectToHost(ui->cpcHost->text(), ui->port->text().toInt());
}

void ClientSimuPage::on_smpsConnBtn_clicked()
{

    smps->connectToHost(ui->smpsHost->text(), ui->port->text().toInt());
}

void ClientSimuPage::on_testConnBtn_clicked()
{
    test->connectToHost(ui->testDevHost->text(), ui->port->text().toInt());
}

void ClientSimuPage::on_cpcDisconnBtn_clicked()
{
   cpc->disconnect();
}

void ClientSimuPage::on_smpsDisconnBtn_clicked()
{
   smps->disconnect();
}

void ClientSimuPage::on_testDevDisconnBtn_clicked()
{
   test->disconnect();
}
