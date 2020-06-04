#ifndef SMPSSTATUSPAGE_H
#define SMPSSTATUSPAGE_H

#include <QWidget>

namespace Ui {
class SmpsStatusPage;
}

class SmpsStatusPage : public QWidget
{
    Q_OBJECT

public:
    explicit SmpsStatusPage(QWidget *parent = nullptr);
    ~SmpsStatusPage();

private:
    Ui::SmpsStatusPage *ui;
};

#endif // SMPSSTATUSPAGE_H
