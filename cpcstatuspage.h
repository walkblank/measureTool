#ifndef CPCSTATUSPAGE_H
#define CPCSTATUSPAGE_H

#include <QWidget>
#include <QTimer>
#include "calibclient.h"

namespace Ui {
class CpcStatusPage;
}

class CpcStatusPage : public QWidget
{
    Q_OBJECT

public:
    explicit CpcStatusPage(QString dev, QWidget *parent = nullptr);
    ~CpcStatusPage();
    void setClient(CalibClient *client);
    void showEvent(QShowEvent *evt);
    void hideEvent(QHideEvent *event);

private slots:
    void onDataReadSig(QString, QMap<QString,QString>);

    void on_setVal127Btn_clicked();

    void on_setVal128Btn_clicked();

    void on_readValBtn_clicked();

    void onCheckTimerTimeout();

private:
    Ui::CpcStatusPage *ui;
    CalibClient *client;
    QString devName;
    QTimer *timer;
};

#endif // CPCSTATUSPAGE_H
