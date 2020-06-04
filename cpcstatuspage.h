#ifndef CPCSTATUSPAGE_H
#define CPCSTATUSPAGE_H

#include <QWidget>

namespace Ui {
class CpcStatusPage;
}

class CpcStatusPage : public QWidget
{
    Q_OBJECT

public:
    explicit CpcStatusPage(QString dev,QWidget *parent = nullptr);
    ~CpcStatusPage();

private:
    Ui::CpcStatusPage *ui;
};

#endif // CPCSTATUSPAGE_H
