#ifndef DATABASERESPONSE_H
#define DATABASERESPONSE_H

#include <QObject>
#include <QSharedPointer>
#include <QStringList>

class DatabaseResponse : public QObject {
  Q_OBJECT

 private:
  typedef HashRecordTable QHash<QString, QSharedPointer<QStringList>>;

 private:
  HashRecordTable Records;
  uint32_t RecordNumber;

 public:
  explicit DatabaseResponse(QObject* parent);
  ~DatabaseResponse();

  void addField(const QString& name, const QSharedPointer<QStringList>);

  void prepareMemoryForRecords(uint32_t num);
  uint32_t getRecordNumber() const;

  bool isEmpty();
  QString getValue(uint32_t record, const QString& field);
};

#endif  // DATABASERESPONSE_H
