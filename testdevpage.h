#ifndef TESTDEVPAGE_H
#define TESTDEVPAGE_H

#include <QWidget>
#include <calibclient.h>
#include <QSettings>

namespace Ui {
class TestDevPage;
}

class TestDevPage : public QWidget
{
    Q_OBJECT

public:
    explicit TestDevPage(QWidget *parent = nullptr);
    ~TestDevPage();
    void setTestClient(CalibClient *client);

private slots:
   void onConnected();
   void onDisconnected();

   void on_connBtn_clicked();

   void onClientData(QString, QMap<QString,QString>);

private:
    Ui::TestDevPage *ui;

    CalibClient *testClient;

    QSettings *settings;
    void saveParam();
};

#endif // TESTDEVPAGE_H
