#ifndef DATABASBUFFER_H
#define DATABASBUFFER_H

#include <QAbstractTableModel>
#include <QMutex>
#include <QMutexLocker>

class DatabaseTableModel : public QAbstractTableModel {
  Q_OBJECT

 private:
  QList<QString> Headers;
  QList<QSharedPointer<QVector<QString>>> Data;

  QMutex Mutex;

 public:
  explicit DatabaseTableModel(QObject* parent);
  ~DatabaseTableModel();

  void build(const QHash<QString, QSharedPointer<QVector<QString>>>* records);
  void clear(void);
  bool isEmpty(void) const;

  // Функционал модели
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  Q_DISABLE_COPY(DatabaseTableModel)
  void deleteAll(void);
};

#endif  // DATABASBUFFER_H
