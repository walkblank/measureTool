#include "datacomparewindow.h"
#include "ui_datacomparewindow.h"

#include <QDebug>

#include <QtXlsx>


DatacompareWindow::DatacompareWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatacompareWindow)
{
    ui->setupUi(this);
}

void DatacompareWindow::setModel(DatacompareModel *model)
{
    this->model = model;
    ui->compareView->setModel(model);
    ui->compareView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    model->setThreshold(ui->threshold->text().toDouble());
}

DatacompareWindow::~DatacompareWindow()
{
    delete ui;
}


void DatacompareWindow::on_setThresholdBtn_clicked()
{
    ui->compareView->setModel(new QStandardItemModel());
    model->setThreshold(ui->threshold->text().toDouble());
    ui->compareView->setModel(model);
}

void DatacompareWindow::on_saveBtn_clicked()
{
   qDebug()<<model->rowCount();
}

void DatacompareWindow::on_printBtn_clicked()
{
   qDebug()<<model->rowCount();
}
