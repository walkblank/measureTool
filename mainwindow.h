#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>

#include <measureserver.h>

#include "datacomparemodel.h"
#include "clientsimupage.h"
#include "datacomparewindow.h"
#include "clientsettingwindow.h"

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
    void onAutoTimeout();

    void on_startBtn_clicked();
    void on_menuBtn_clicked();
    void on_startCalibBtn_clicked();
    void on_upperValue_textChanged(const QString &arg1);
    void on_lowerValue_textChanged(const QString &arg1);
    void on_averTime_textChanged(const QString &arg1);
    void on_saveBtn_clicked();
    void onSigClientConn(QString ip);
    void onSigClientDisconn(QString ip);
    void onClientData(QString, QMap<QString,QString>);
    void onClientRet(QString,QString, QMap<QString,QString>);

    void on_clientListBtn_clicked();

    void on_setMeter_textChanged(const QString &arg1);

    void on_tableBtn_clicked();

private:
    Ui::MainWindow *ui;
    MeasureServer *server;
    QChart  *m_chart;
    QChartView *m_chartView;

    bool calib = false;
    bool autom = false;
    int averageTime = 0;

    QLineSeries *cpc1slineSeries;
    QLineSeries *cpc10slineSeries;
    QLineSeries *testDevLineSeries;

    ClientSimuPage *simuPage;
    QLineSeries *upperCalib;
    QLineSeries *lowerCalib;

    QTimer *timer, *autoTimer;
    QMap<QString, QList<qreal>> pointMaps;
    QList<qreal> cpcPt1;
    QList<qreal> cpcPt10;
    QList<qreal> cpcTest;

    QSettings *settings;

    QMap<QString, QString> addrList;
    QMap<QString, CalibClient*> connList;
    QMap<QString, QLineEdit*> addrLineList;

    QList<QString> enterClsDevs;
    QList<QString> enterAutoDevs;

    DatacompareModel *model;
    QList<QString> datetimeList;
    QList<double>  cpcVList;
    QList<double>  testVList;
    QList<double>  errVList;
    QList<QStandardItem*> timeList;
    QList<QStandardItem*> cpcList;
    QList<QStandardItem*> testList;
    QList<QStandardItem*> errList;
    DatacompareWindow *compareWindow;
    ClientSettingWindow *settingWindow;

private:
    void initChartsView();
    void loadSettings();
    void getLocalIpAddr();
    bool checkIp(QString ip);
};
#endif // MAINWINDOW_H
