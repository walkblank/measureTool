#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QMutex>
#include <QtXlsx>

#include "measureserver.h"
#include "datacomparemodel.h"
#include "clientsimupage.h"
#include "datacomparewindow.h"
#include "clientsettingwindow.h"
#include "settingpage.h"
#include "cpcstatuspage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString dev, QWidget *parent = nullptr);
    void setClient(CalibClient *client);
    void setSmpsClient(CalibClient *client);
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

    void onClientData(QString, QMap<QString,QString>);
    void onClientRet(QString,QString, QMap<QString,QString>);

    void on_clientListBtn_clicked();

    void on_setMeter_textChanged(const QString &arg1);

    void on_tableBtn_clicked();

    void on_cpcConnBtn_clicked();

    void onConnected();
    void onDisconnected();

    void on_sampleInterval_textChanged(const QString &arg1);

    void on_sampleCnt_textChanged(const QString &arg1);
    void onXishiSig(QString val, QString devName);

private:
    Ui::MainWindow *ui;

    QString deviceName;
    CalibClient *cpcClient, *smpsCient;
    QChart  *m_chart;
    QChartView *m_chartView;
    QMutex mutex;
    QXlsx::Document *currXlsx = nullptr;

    bool calib = false;
    bool autom = false;
    bool isXSSet = false;
    int averageTime = 0;
    int sampleInterval = 10;
    int sampleCnt = 10;
    int writeCnt = 0;

    QList<double> tmpPt,tmpSavePt;

    QMap<QString,QString> xishiVals;

    QLineSeries *cpc1slineSeries;
    QLineSeries *cpc10slineSeries;

    ClientSimuPage *simuPage;
    QLineSeries *upperCalib;
    QLineSeries *lowerCalib;

    QTimer *timer, *autoTimer;
    QMap<QString, QList<qreal>> pointMaps;
    QList<qreal> cpcPt1;
    QList<qreal> cpcPt10;

    QSettings *settings;
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
    SettingPage *settingPage;
    CpcStatusPage *stPage;

    QMap<QString,bool> classReadyDevMap, autoReadyDevMap;

private:
    void initChartsView();
    void loadSettings();
    bool checkIp(QString ip);

    void waitSomeTime(int ms);
    void xlsxFileWriteHeader(QXlsx::Document *file);
    void xlsxFilwWriteRecord(QXlsx::Document *file, int row, QString time, QString count, QString flowRate, QString md1, QString md2);
};
#endif // MAINWINDOW_H
