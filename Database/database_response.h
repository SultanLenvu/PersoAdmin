#ifndef DATABASERESPONSE_H
#define DATABASERESPONSE_H

#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QVector>

class DatabaseRecordTable : public QObject {
  Q_OBJECT

 private:
  QHash<QString, QSharedPointer<QVector<QString>>> Records;
  uint32_t RecordNumber;

 public:
  explicit DatabaseRecordTable(QObject* parent);
  ~DatabaseRecordTable();

  void addField(const QString& name);
  void addField(const QString& name, const QSharedPointer<QVector<QString>>);
  void addField(const QString& name, uint32_t recordNumber);

  void addFieldValue(const QString& field, const QString& value);

  uint32_t getRecordNumber(void) const;
  uint32_t getFieldNumber(void) const;

  bool isEmpty(void);
  QString getValue(uint32_t record, const QString& field);

  void clear(void);
};

#endif  // DATABASERESPONSE_H
