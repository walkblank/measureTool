#include "datacomparewindow.h"
#include "ui_datacomparewindow.h"

#include <QDebug>
#include <QMessageBox>

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
    int row = model->rowCount();
    if(row <= 0)
        return;

//    QList<QStandardItem*> rowItems;
//    for(int i = 0; i < 20; i ++)
//    {
//        rowItems.clear();
//        rowItems << new QStandardItem("99999")
//                 << new QStandardItem("0.5")
//                 << new QStandardItem("0.3")
//                 << new QStandardItem("0.2");
//        model->appendRow(rowItems);
//    }
//    row = model->rowCount();

    QString filename =  QString(QApplication::applicationDirPath()
                                .append(QString("/%1.xlsx")
                                        .arg(QDateTime::currentDateTime().toString("yyyy_MM_dd-hhmmss"))));
    qDebug()<<"file name " << filename;
    QXlsx::Document xlsfile(filename);
    xlsfile.write(1, 1, "index");
    xlsfile.write(1, 2, "time");
    xlsfile.write(1, 3, "cpc value");
    xlsfile.write(1, 4, "test value");
    xlsfile.write(1, 5, "error");

    for(int i = 0; i < row; i ++)
    {
         xlsfile.write(i+2, 1, i);
         xlsfile.write(i+2, 2, model->item(i, 0)->text());
         xlsfile.write(i+2, 3, model->item(i, 1)->text());
         xlsfile.write(i+2, 4, model->item(i, 2)->text());
         xlsfile.write(i+2, 5, model->item(i, 3)->text());
    }

    QMessageBox::information(this, tr("提示"), tr("数据已保存: %1").arg(filename), QMessageBox::Ok);
    xlsfile.save();
}

void DatacompareWindow::on_printBtn_clicked()
{
//    QList<QStandardItem*> rowItems;
//    for(int i = 0; i < 20; i ++)
//    {
//        rowItems.clear();
//        rowItems << new QStandardItem("99999")
//                 << new QStandardItem("0.5")
//                 << new QStandardItem("0.3")
//                 << new QStandardItem("0.2");
//        model->appendRow(rowItems);
//    }
    int row = model->rowCount();
    if(row <= 0)
        return;

    //    QStringList html;
    //    qDebug()<<model->rowCount();

    int rowCount = model->rowCount();
    html.clear();
    QString title = "compare table";
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "index" << "time" << "cpc" << "testVal" << "error";
    columnWidths << 80 << 140 << 140 << 140 << 140;
    int columnCnt = columnNames.count();
    qDebug()<<"column count" << columnCnt;
    qDebug()<<"model rowCount" << model->rowCount();

    html.append("<table border='0.5' cellspacing='0' cellpadding='1'>");

    html.append(QString("<tr><td align='center' style='vertical-align:middle;font-weight:bold;' colspan='%1'>")
                .arg(4));
    html.append(title);
    html.append("</td></tr>");

    if(columnCnt > 0)
    {
        html.append("<tr>");
        for (int i = 0; i < columnCnt; i++) {
            html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                        .arg(columnWidths.at(i)));
            html.append(columnNames.at(i));
            html.append("</td>");
        }
        html.append("</tr>");
    }

    if(rowCount > 0)
    {
        for(int i = 0; i < rowCount; i ++)
        {
            html.append("<tr>");

            for(int j = 0; j < columnCnt; j ++)
            {
                html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                            .arg(columnWidths.at(j)));
                if(j == 0)
                    html.append(QString("%1").arg(i));
                else
                    html.append(model->item(i, j - 1)->text());
                html.append("</td>");
            }
        }

    }


    QTextDocument text;
    QPrinter printer;
    //    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFormat(QPrinter::NativeFormat);
//    printer.setOutputFileName("test.pdf");


    QPrintPreviewDialog preview(&printer);
    preview.setWindowTitle("打印预览");

    connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printView(QPrinter *)));

    preview.exec();
}

void DatacompareWindow::printView(QPrinter *printer)
{
    QTextDocument txt;
    txt.setHtml(html.join(""));
    txt.print(printer);
}
