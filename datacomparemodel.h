#ifndef DATACOMPAREMODEL_H
#define DATACOMPAREMODEL_H

#include<QStandardItemModel>

typedef enum
{
    DATE_IDX = 0,
    CPC_DATA_IDX,
    TESTDEV_DATA_IDX,
    CMP_STA_IDX
} datamodel_item_index;


class DatacompareModel : public QStandardItemModel
{
public:
    explicit DatacompareModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    void setThreshold(double value) {threshold = value;}

    void insertCPCData(QString time, QString cpc);
    void insertTestDevData(QString value);

private:
    double threshold;
};

#endif // DATACOMPAREMODEL_H
