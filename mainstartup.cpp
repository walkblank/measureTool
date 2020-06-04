#include "mainstartup.h"
#include "ui_mainstartup.h"

MainStartUp::MainStartUp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainStartUp)
{
    ui->setupUi(this);
    setFixedSize(width(), height());

    cpc1Window = new MainWindow("CPC1");
    cpc2Window = new MainWindow("CPC2");
    mide1Page = new MideSettingPage("MD19-3E@1");
    mide2Page = new MideSettingPage("MD19-3E@2");

    mdClient1 = new Md19Client();
    mdClient2 = new Md19Client();
    connect(mdClient1, SIGNAL(connected()), this, SLOT(onMdClientConnect()));
}

void MainStartUp::onMdClientConnect()
{
    qDebug()<<"connect";
}
void MainStartUp::showEvent(QShowEvent *evt)
{
   Q_UNUSED(evt)
}
MainStartUp::~MainStartUp()
{
    delete ui;
}

void MainStartUp::on_openMd1Btn_clicked()
{
//    mdClient1->connectToHost("192.168.199.147", 3320);
    mide1Page->show();
}

void MainStartUp::on_openMd2Btn_clicked()
{
    mide2Page->show();
}

void MainStartUp::on_openCpc1Btn_clicked()
{
    cpc1Window->show();
}

void MainStartUp::on_openCpc2Btn_clicked()
{
    cpc2Window->show();
}

void MainStartUp::on_openSmpsBtn_clicked()
{

}
