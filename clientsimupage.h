#ifndef CLIENTSIMUPAGE_H
#define CLIENTSIMUPAGE_H

#include <QWidget>
#include <calibclient.h>
#include <QSettings>

namespace Ui {
class ClientSimuPage;
}

class ClientSimuPage : public QWidget
{
    Q_OBJECT

public:
    explicit ClientSimuPage(QWidget *parent = nullptr);
    ~ClientSimuPage();

private slots:
    void on_cpcConnBtn_clicked();

    void on_smpsConnBtn_clicked();

    void on_testConnBtn_clicked();

    void on_cpcDisconnBtn_clicked();

    void on_smpsDisconnBtn_clicked();

    void on_testDevDisconnBtn_clicked();

private:
    Ui::ClientSimuPage *ui;
    CalibClient *cpc, *smps, *test;
};

#endif // CLIENTSIMUPAGE_H
