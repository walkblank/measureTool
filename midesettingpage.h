#ifndef MIDESETTINGPAGE_H
#define MIDESETTINGPAGE_H

#include <QWidget>

namespace Ui {
class MideSettingPage;
}

class MideSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit MideSettingPage(QString dev, QWidget *parent = nullptr);
    ~MideSettingPage();

private:
    Ui::MideSettingPage *ui;
};

#endif // MIDESETTINGPAGE_H
