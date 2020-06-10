#include "smpsstatuspage.h"
#include "ui_smpsstatuspage.h"

#include <QDebug>

SmpsStatusPage::SmpsStatusPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmpsStatusPage)
{
    ui->setupUi(this);
}

SmpsStatusPage::~SmpsStatusPage()
{
    delete ui;
}

void SmpsStatusPage::on_smpsConnBtn_clicked()
{
    qDebug()<<"smsp conn";
}

void SmpsStatusPage::on_smpsReadParamBtn_clicked()
{
    qDebug()<<"smsp read";
}
