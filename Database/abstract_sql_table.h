#ifndef ABSTRACTSQLTABLE_H
#define ABSTRACTSQLTABLE_H

#include <QObject>

class AbstractSqlTable : public QObject {
  Q_OBJECT

 public:
  explicit AbstractSqlTable(QObject* parent);
  virtual ~AbstractSqlTable();

  virtual bool init() = 0;
  virtual void applySettings() = 0;

  // Create
  virtual bool createRecords(
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const = 0;

  // Read
  virtual bool readRecords(
      const QString& conditions,
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const = 0;
  virtual bool readLastRecord(
      QHash<QString, QSharedPointer<QVector<QString>>>& record) const = 0;

  // Update
  virtual bool updateRecords(
      const QString& condition,
      const QHash<QString, QString>& newValues) const = 0;

  // Delete
  virtual bool deleteRecords(const QString& condition) const = 0;
  virtual bool clear(void) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractSqlTable)

 signals:
  void logging(const QString& log);
};

#endif  // ABSTRACTSQLTABLE_H
