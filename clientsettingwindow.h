#ifndef CLIENTSETTINGWINDOW_H
#define CLIENTSETTINGWINDOW_H

#include <QWidget>
#include <QShowEvent>

namespace Ui {
class ClientSettingWindow;
}

class ClientSettingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClientSettingWindow(QWidget *parent = nullptr);
    ~ClientSettingWindow();
protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::ClientSettingWindow *ui;
};

#endif // CLIENTSETTINGWINDOW_H
