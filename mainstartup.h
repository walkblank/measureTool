#ifndef MAINSTARTUP_H
#define MAINSTARTUP_H

#include <QMainWindow>
#include "mainwindow.h"
#include "midesettingpage.h"
#include "md19client.h"
#include "smpsstatuspage.h"
#include "calibclient.h"
#include "testdevpage.h"

namespace Ui {
class MainStartUp;
}

class MainStartUp : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainStartUp(QWidget *parent = nullptr);
    ~MainStartUp();

    void showEvent(QShowEvent *evt);

private slots:
    void on_openMd1Btn_clicked();

    void on_openMd2Btn_clicked();

    void on_openCpc1Btn_clicked();

    void on_openCpc2Btn_clicked();

    void on_openSmpsBtn_clicked();

    void onMdClientConnect();

    void on_startMd1Btn_clicked();
    void on_startMd2Btn_clicked();

    void onXishiValSig(QString val, QString devName);
    void onSigData(int cmd, QVariant var, QVariant var1, int reg);
    void onCpcDataSig(QString type, QMap<QString,QString>);

    void on_openTestDevBtn_clicked();

    void onTestQuerySignal();

    void queryRoutine();

private:
    Ui::MainStartUp *ui;

    MainWindow *cpc1Window, *cpc2Window;
    MideSettingPage *mide1Page, *mide2Page;
    SmpsStatusPage *smpsPage;
    TestDevPage *testDevPage;

    Md19Client *mdClient1, *mdClient2;
    CalibClient *cpc1Client, *cpc2Client;
    CalibClient *smpsClient, *testClient;

    MeasureServer *server, *server1, *server2;

    QList<QPushButton *> mdStartBtns;

    QTimer *queryTimer;
};

#endif // MAINSTARTUP_H
