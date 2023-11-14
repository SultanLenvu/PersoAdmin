#ifndef SqlRecordCreationForm_H
#define SqlRecordCreationForm_H

#include <QHash>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QVector>

class SqlRecordCreationForm : public QObject {
  Q_OBJECT

 private:
  template <typename T>
  using VectorPointer = QSharedPointer<QVector<T>>;

 private:
  QVector<QString> Fields;
  QHash<QString, int32_t> FieldIndex;
  VectorPointer<VectorPointer<QString>> Records;

 public:
  explicit SqlRecordCreationForm(QObject* parent = nullptr);
  ~SqlRecordCreationForm();

  QString getField(uint32_t i) const;
  QString getValue(uint32_t record, const QString& field) const;
  QString getValue(uint32_t record, uint32_t field) const;
  int32_t getRecordNumber(void) const;
  int32_t getFieldNumber(void) const;
  void generate(QString& form) const;

  void addRecord(const QHash<QString, QString>& record);
  void addFieldColumn(const QString& name,
                      const QSharedPointer<QVector<QString>>& values);
  void addFieldValue(const QString& name, const QString& value);
  void clear();

 private:
  Q_DISABLE_COPY_MOVE(SqlRecordCreationForm)
 signals:
};

#endif  // SqlRecordCreationForm_H
