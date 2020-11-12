#ifndef MIDESETTINGPAGE_H
#define MIDESETTINGPAGE_H

#include <QWidget>
#include <QObject>
#include <QSettings>
#include <QShowEvent>
#include <QHideEvent>

#include "md19client.h"

namespace Ui {
class MideSettingPage;
}

class MideSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit MideSettingPage(QString dev, QWidget *parent = nullptr);
    ~MideSettingPage();
    void setClient(Md19Client *cli);

    void showEvent(QShowEvent *evt);
    void closeEvent(QCloseEvent *evt);

signals:
    void sigXishiVal(QString, QString);

private slots:
    void on_connBtn_clicked();
    void on_setTempBtn_clicked();
    void onClientConnected();
    void onClienDisconnected();
    void on_saveBtn_clicked();
    void onSigData(int cmd, QVariant, QVariant, int reg = 0);

    void on_readTempBtn_clicked();

    void on_remoteSwiBtn_clicked();

private:
    Ui::MideSettingPage *ui;
    Md19Client *client;
    QString devName;
    QSettings *setting;


    void saveParam();
};

#endif // MIDESETTINGPAGE_H
