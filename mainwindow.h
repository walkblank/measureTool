#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <measureserver.h>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startBtn_clicked();
    void onTimerTimeout();

private:
    Ui::MainWindow *ui;
    MeasureServer *server;
    QChart  *m_chart;
    QChartView *m_chartView;
    QLineSeries *lineSeries;
    QTimer *timer;
private:
    void initChartsView();
};
#endif // MAINWINDOW_H
