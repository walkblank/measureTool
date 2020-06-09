#ifndef MIDESETTINGPAGE_H
#define MIDESETTINGPAGE_H

#include <QWidget>
#include <QSettings>

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

private slots:
    void on_connBtn_clicked();
    void on_setTempBtn_clicked();
    void onClientConnected();
    void onClienDisconnected();
    void on_saveBtn_clicked();

    void saveParam();

private:
    Ui::MideSettingPage *ui;
    Md19Client *client;
    QString devName;
    QSettings *setting;
};

#endif // MIDESETTINGPAGE_H
