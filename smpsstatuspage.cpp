#include "smpsstatuspage.h"
#include "ui_smpsstatuspage.h"

#include <QDebug>
#include <QMessageBox>

SmpsStatusPage::SmpsStatusPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmpsStatusPage)
{
    ui->setupUi(this);

    setWindowTitle("SMPS状态界面");
    settings = new QSettings("setting.ini", QSettings::IniFormat);
    if(settings->contains("smpsAddr"))
        ui->smpsAddr->setText(settings->value("smpsAddr").toString());
    if(settings->contains("smpsPort"))
        ui->smpsPort->setText(settings->value("smpsPort").toString());
    setFixedSize(width(), height());
}

SmpsStatusPage::~SmpsStatusPage()
{
    delete ui;
}

bool SmpsStatusPage::checkIp(QString ip)
{
    QRegExp rx2("^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$");
    if(!rx2.exactMatch(ip))
        return false;
    return true;
}

void SmpsStatusPage::saveParam()
{
    if(!checkIp(ui->smpsAddr->text()))
    {
        QMessageBox::warning(this, "提示", tr("invalid ip address"), QMessageBox::Ok);
        return;
    }

    settings->setValue("smpsAddr", ui->smpsAddr->text());
    int port = ui->smpsPort->text().toInt();
    if(port < 0 || port > 65535)
    {
        QMessageBox::warning(this, "提示", tr("invalid listen port"), QMessageBox::Ok);
        return;
    }
    settings->setValue("smpsPort", ui->smpsPort->text());
}

void SmpsStatusPage::setClient(CalibClient *client)
{
   smpsClient = client;

    connect(smpsClient, SIGNAL(sigConnected()), this, SLOT(onConnected()));
    connect(smpsClient, SIGNAL(sigDisConnected()), this, SLOT(onDisconnected()));
    connect(smpsClient, SIGNAL(sigReadData(QString, QMap<QString,QString>)),
            this, SLOT(onClientData(QString, QMap<QString,QString>)));
}

void SmpsStatusPage::onConnected()
{
   ui->smpsAddr->setStyleSheet("background-color: rgb(0, 255, 127);");
   saveParam();
}

void SmpsStatusPage::onDisconnected()
{
  ui->smpsAddr->setStyleSheet("background-color: rgb(255, 255, 127);");
}

void SmpsStatusPage::onClientData(QString type, QMap<QString,QString> values)
{
    Q_UNUSED(type)
    CalibClient *client = static_cast<CalibClient*>(sender());
    if(client == smpsClient)
    {
        if(values.contains("0"))
        {
            ui->val0->setText(values["0"]);
            ui->val1->setText(values["1"]);
            ui->val2->setText(values["2"]);
            ui->val3->setText(values["3"]);
            ui->val4->setText(values["4"]);
            ui->val8->setText(values["8"]);
            ui->val9->setText(values["9"]);
            ui->val20->setText(values["20"]);
            ui->val21->setText(values["21"]);
            ui->val26->setText(values["26"]);
        }
    }
    else
    {
        if(values.contains("28"))
        {
            ui->cpc27->setText(values["27"]);
            ui->cpc28->setText(values["28"]);
        }
    }
}

void SmpsStatusPage::on_smpsConnBtn_clicked()
{
    qDebug()<<"smsp conn";
    if(smpsClient->state() != QAbstractSocket::ConnectedState)
        smpsClient->connectToHost(ui->smpsAddr->text(),  ui->smpsPort->text().toUInt());
    else
        smpsClient->disconnect();
}

void SmpsStatusPage::on_smpsReadParamBtn_clicked()
{
    if(smpsClient->state() != QAbstractSocket::ConnectedState)
        return;
    qDebug()<<"smsp read";
    QList<QString> cmds;
    cmds << "3" << "8" << "9" << "1" << "2"
         << "21" << "20" << "0" << "4" << "26";
    smpsClient->getValue(cmds);
}

void SmpsStatusPage::on_saveBtn_clicked()
{
    saveParam();
}
