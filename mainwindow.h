#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>

#include <measureserver.h>
#include "clientsimupage.h"

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
    void onTimerTimeout();

    void on_startBtn_clicked();
    void on_menuBtn_clicked();
    void on_startCalibBtn_clicked();
    void on_upperValue_textChanged(const QString &arg1);
    void on_lowerValue_textChanged(const QString &arg1);
    void on_cnValue_textChanged(const QString &arg1);
    void on_averTime_textChanged(const QString &arg1);
    void on_saveBtn_clicked();
    void on_setMeter_editingFinished();

    void onSigClientConn(QString ip);
    void onSigClientDisconn(QString ip);
    void onClientData(QString, QString);

    void on_clientListBtn_clicked();

private:
    Ui::MainWindow *ui;
    MeasureServer *server;
    QChart  *m_chart;
    QChartView *m_chartView;

    QLineSeries *cpc1slineSeries;
    QLineSeries *cpc10slineSeries;

    QLineSeries *testDevLineSeries;
    ClientSimuPage *simuPage;

    QLineSeries *upperCalib;
    QLineSeries *lowerCalib;

    QTimer *timer;
    QList<qreal> pt, pt1;
    QList<qreal> spt10s;

    QSettings *settings;

    QMap<QString, QString> addrList;
    QMap<QString, CalibClient*> connList;
    QMap<QString, QLineEdit*> addrLineList;

private:
    void initChartsView();
    void loadSettings();
    void getLocalIpAddr();
    bool checkIp(QString ip);
};
#endif // MAINWINDOW_H
