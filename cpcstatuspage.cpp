#include "cpcstatuspage.h"
#include "ui_cpcstatuspage.h"

CpcStatusPage::CpcStatusPage(QString dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpcStatusPage)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1设备状态").arg(dev));
    setFixedSize(width(), height());
}

CpcStatusPage::~CpcStatusPage()
{
    delete ui;
}
