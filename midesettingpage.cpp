#include "midesettingpage.h"
#include "ui_midesettingpage.h"

MideSettingPage::MideSettingPage(QString dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MideSettingPage)
{
    ui->setupUi(this);
    setWindowTitle(dev);
}

MideSettingPage::~MideSettingPage()
{
    delete ui;
}
