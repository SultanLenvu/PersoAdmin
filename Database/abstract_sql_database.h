#ifndef ABSTRACTSQLDATABASE_H
#define ABSTRACTSQLDATABASE_H

#include <QObject>
#include <QSharedPointer>

class AbstractSqlDatabase : public QObject {
  Q_OBJECT

 public:
  explicit AbstractSqlDatabase(QObject* parent);
  virtual ~AbstractSqlDatabase();

  virtual void applySettings() = 0;

  virtual bool connect(void) = 0;
  virtual void disconnect(void) = 0;
  virtual bool isConnected(void) = 0;

  virtual bool openTransaction(void) const = 0;
  virtual bool closeTransaction(void) const = 0;
  virtual bool abortTransaction(void) const = 0;

  virtual Qt::SortOrder getCurrentOrder() const = 0;
  virtual void setCurrentOrder(Qt::SortOrder order) = 0;

  virtual uint32_t getRecordMaxCount(void) const = 0;
  virtual void setRecordMaxCount(uint32_t count) const = 0;

  // Create
  virtual bool createRecord(
      const QString& table,
      QVector<QHash<QString, QString>>& records) const = 0;

  // Read
  virtual bool readRecords(
      const QString& table,
      const QHash<QString, QString>& searchValues,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const = 0;
  virtual bool readMergedRecords(
      const QStringList& tables,
      const QHash<QString, QString>& searchValues,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const = 0;

  // Update
  virtual bool updateRecords(const QString& table,
                             const QString& condition,
                             QHash<QString, QString>& newValues) const = 0;

  // Delete
  virtual bool deleteRecords(
      const QString& table,
      const QHash<QString, QString>& searchValues) const = 0;
  virtual bool clearTable(const QString& table) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractSqlDatabase)

 signals:
  void logging(const QString& log);
  void disconnected(void);
};

#endif  // ABSTRACTSQLDATABASE_H
