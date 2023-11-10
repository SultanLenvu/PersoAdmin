#ifndef ABSTRACTSQLTABLE_H
#define ABSTRACTSQLTABLE_H

#include <QObject>

class AbstractSqlTable : public QObject {
  Q_OBJECT

 public:
  explicit AbstractSqlTable(QObject* parent);
  virtual ~AbstractSqlTable();

  virtual bool init() = 0;

  // Create
  virtual bool createRecords(
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const = 0;

  // Read
  virtual bool readRecords(
      const QHash<QString, QString>& searchValues,
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const = 0;

  // Update
  virtual bool updateRecords(const QString& condition,
                             QHash<QString, QString>& newValues) const = 0;

  // Delete
  virtual bool deleteRecords(const QString& condition) const = 0;
  virtual bool clearTable(void) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractSqlTable)

 signals:
  void logging(const QString& log);
};

#endif  // ABSTRACTSQLTABLE_H
