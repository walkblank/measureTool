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
    void insertCPCData(double value);
    void insertTestDevData(double value);
};

#endif // DATACOMPAREMODEL_H
