#ifndef MAINSTARTUP_H
#define MAINSTARTUP_H

#include <QMainWindow>
#include "mainwindow.h"

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

private:
    Ui::MainStartUp *ui;

    MainWindow *cpc2Window;
    MainWindow *cpc1Window;
};

#endif // MAINSTARTUP_H
