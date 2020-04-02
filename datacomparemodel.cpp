#include "datacomparemodel.h"

DatacompareModel::DatacompareModel(QObject *parent)
    : QObject(parent)
{
    QStringList header;
    header << "";
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

//          QStandardItem *itm1 =  item(index.row(), DB_CMPST_IDX);
//          if(itm1 != nullptr)
//          {
//              if(!itm1->text().compare("1"))
//                  return QVariant(QColor(Qt::yellow));
//              if(!itm1->text().compare("2"))
//                  return QVariant(QColor(Qt::gray));
//              if(!itm1->text().compare("3"))
//                  return QVariant(QColor(Qt::cyan));
//              if(!itm1->text().compare("4"))
//                  return QVariant(QColor(Qt::darkCyan));
//          }
      }
      return QStandardItemModel::data(index, role);
}
