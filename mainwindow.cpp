#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_chart(new QChart())
{
    ui->setupUi(this);
    server = new MeasureServer();
    initChartsView();

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimerTimeout()
{
    static int cnt = 0;
    qDebug()<<"timer timeout";
    if(cnt < 10)
        cnt ++;
    else
    {
        cnt = 10;
        lineSeries->remove(0);
    }
        lineSeries->append(cnt, QRandomGenerator::global()->bounded(1.00));
}

void MainWindow::initChartsView()
{
    lineSeries = new QLineSeries();
    lineSeries->setName("cpc");

    m_chartView = new QChartView(m_chart);
    m_chartView->setMinimumSize(1000, 600);

//    lineSeries->append(0, 1.00);
//    lineSeries->append(10, 0.51);
//    lineSeries->append(20, 0.62);
//    lineSeries->append(30, 0.22);
//    lineSeries->append(40, 0.62);
//    lineSeries->append(50, 0.72);
    qDebug()<<lineSeries->count();

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 10);
    xAxis->setTickCount(20);
    xAxis->setMinorTickCount(1);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText("time");

    QValueAxis *yAxis = new QValueAxis;
    yAxis->setRange(-1, 1);
    yAxis->setTickCount(10);
    yAxis->setLabelFormat("%.2f");
    yAxis->setMinorTickCount(1);
    yAxis->setTitleText("value");

    m_chart->addSeries(lineSeries);
    m_chart->addAxis(xAxis, Qt::AlignBottom);
    m_chart->addAxis(yAxis, Qt::AlignLeft);
    lineSeries->attachAxis(yAxis);
    lineSeries->attachAxis(xAxis);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartViewLayout->addWidget(m_chartView);
}

void MainWindow::on_startBtn_clicked()
{
    timer->start(1000);
}
