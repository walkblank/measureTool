#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QWidget>
#include "calibclient.h"

namespace Ui {
class SettingPage;
}

class SettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingPage(QMap<QString,CalibClient*> &map, QWidget *parent = nullptr);
    ~SettingPage();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_readSMPSBtn_clicked();

    void on_readCPCBtn_clicked();

    void onClientData(QString, QMap<QString,QString>);

private:
    Ui::SettingPage *ui;
    QMap<QString, CalibClient*> &clientList;
};

#endif // SETTINGPAGE_H
