#ifndef DATACOMPAREWINDOW_H
#define DATACOMPAREWINDOW_H

#include <QWidget>

#include "datacomparemodel.h"

namespace Ui {
class DatacompareWindow;
}

class DatacompareWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DatacompareWindow(QWidget *parent = nullptr);
    ~DatacompareWindow();
    void setModel(DatacompareModel *model);

private slots:

    void on_setThresholdBtn_clicked();

    void on_saveBtn_clicked();

    void on_printBtn_clicked();

private:
    Ui::DatacompareWindow *ui;
    DatacompareModel *model;
};

#endif // DATACOMPAREWINDOW_H
