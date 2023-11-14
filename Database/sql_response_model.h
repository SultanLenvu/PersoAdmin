#ifndef DATABASERESPONSE_H
#define DATABASERESPONSE_H

#include <QAbstractTableModel>
#include <QHash>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>
#include <QSqlQuery>
#include <QVector>

class SqlResponseModel : public QAbstractTableModel {
  Q_OBJECT

 private:
  QVector<QString> Fields;
  QHash<QString, int32_t> FieldPosition;
  QVector<QSharedPointer<QVector<QString>>> Values;

  QMutex Mutex;

 public:
  explicit SqlResponseModel(QObject* parent = nullptr);
  ~SqlResponseModel();

  uint32_t getRecordNumber(void) const;
  uint32_t getFieldNumber(void) const;

  bool isEmpty(void) const;
  QString getValue(uint32_t record, const QString& field) const;
  QString getFirstValue(const QString& field) const;

  void extractRecords(QSqlQuery& request);
  void clear(void);

  // Интерфейс модели
 public:
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
};

#endif  // DATABASERESPONSE_H
