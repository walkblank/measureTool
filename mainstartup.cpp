#include "mainstartup.h"
#include "ui_mainstartup.h"

MainStartUp::MainStartUp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainStartUp)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    cpc2Window = new MainWindow(this);
    cpc1Window = new MainWindow(this);
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

}

void MainStartUp::on_openMd2Btn_clicked()
{

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
