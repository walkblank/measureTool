#include "clientsettingwindow.h"
#include "ui_clientsettingwindow.h"

ClientSettingWindow::ClientSettingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientSettingWindow)
{
    ui->setupUi(this);
}

void ClientSettingWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    setFixedSize(size().width(), size().height());
}

ClientSettingWindow::~ClientSettingWindow()
{
    delete ui;
}
