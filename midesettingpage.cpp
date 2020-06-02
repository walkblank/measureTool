#include "midesettingpage.h"
#include "ui_midesettingpage.h"

MideSettingPage::MideSettingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MideSettingPage)
{
    ui->setupUi(this);
}

MideSettingPage::~MideSettingPage()
{
    delete ui;
}
