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

    mdClient1 = new Md19Client();
    mdClient2 = new Md19Client();

    mide1Page = new MideSettingPage("MD19-3E@1");
    mide2Page = new MideSettingPage("MD19-3E@2");

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
    if(mide1Page->isVisible())
        mide1Page->raise();
    else
        mide1Page->show();
}

void MainStartUp::on_openMd2Btn_clicked()
{
    if(mide2Page->isVisible())
        mide2Page->raise();
    else
        mide2Page->show();
}

void MainStartUp::on_openCpc1Btn_clicked()
{
    if(cpc1Window->isVisible())
        cpc1Window->raise();
    else
        cpc1Window->show();
}

void MainStartUp::on_openCpc2Btn_clicked()
{
    if(cpc2Window->isVisible())
        cpc2Window->raise();
    else
        cpc2Window->show();
}

void MainStartUp::on_openSmpsBtn_clicked()
{

}
