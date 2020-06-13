#ifndef SMPSSTATUSPAGE_H
#define SMPSSTATUSPAGE_H

#include <QWidget>
#include <QSettings>

#include "calibclient.h"

namespace Ui {
class SmpsStatusPage;
}

class SmpsStatusPage : public QWidget
{
    Q_OBJECT
public:
    explicit SmpsStatusPage(QWidget *parent = nullptr);
    ~SmpsStatusPage();
    void setClient(CalibClient *client);

private slots:
    void on_smpsConnBtn_clicked();

    void on_smpsReadParamBtn_clicked();
    void onClientData(QString, QMap<QString,QString>);
    void onConnected();
    void onDisconnected();

    void on_saveBtn_clicked();

private:
    Ui::SmpsStatusPage *ui;
    CalibClient *smpsClient;
    QSettings *settings;

    void saveParam();
    bool checkIp(QString ip);
};

#endif // SMPSSTATUSPAGE_H
