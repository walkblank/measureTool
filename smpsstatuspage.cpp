#include "smpsstatuspage.h"
#include "ui_smpsstatuspage.h"

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
