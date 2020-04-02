#include "datacomparemodel.h"

DatacompareModel::DatacompareModel(QObject *parent)
    : QStandardItemModel(parent)
{
    QStringList header;
    header << "Time" <<"CPC Value" << "Test Value" << "Error";
    setHorizontalHeaderLabels(header);
}

QVariant DatacompareModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::BackgroundColorRole)
      {
          QStandardItem *itm = item(index.row(), CMP_STA_IDX);
          if(itm != nullptr && !itm->text().isEmpty())
          {
              if(!itm->text().compare("0"))
                  return QVariant(QColor(Qt::green));
              return QVariant(QColor(Qt::red));
          }
      }
      return QStandardItemModel::data(index, role);
}
