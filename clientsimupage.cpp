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
