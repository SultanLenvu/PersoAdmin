#include "sql_record_creation_form.h"

SqlRecordCreationForm::SqlRecordCreationForm(QObject* parent)
    : QObject{parent} {}

SqlRecordCreationForm::~SqlRecordCreationForm() {}

QString SqlRecordCreationForm::getField(uint32_t i) const {
  return Fields.at(i);
}

QString SqlRecordCreationForm::getValue(uint32_t record,
                                        const QString& field) const {
  return Records->at(record)->at(FieldIndex.value(field));
}

QString SqlRecordCreationForm::getValue(uint32_t record, uint32_t field) const {
  return Records->at(record)->at(field);
}

int32_t SqlRecordCreationForm::getRecordNumber() const {
  return Records->size();
}

int32_t SqlRecordCreationForm::getFieldNumber() const {
  return Fields.size();
}

void SqlRecordCreationForm::generate(QString& form) const {
  for (int32_t i = 0; i < Records->size(); i++) {
    form.append("(");
    for (int32_t j = 0; j < Records->at(i)->size(); j++) {
      form.append(Records->at(i)->at(j));
    }
    form.append("),\n");
  }
  form.chop(2);
}

void SqlRecordCreationForm::addRecord(const QHash<QString, QString>& record) {
  for (QHash<QString, QString>::const_iterator it = record.constBegin();
       it != record.constEnd(); it++) {
    addFieldValue(it.key(), it.value());
  }
}

void SqlRecordCreationForm::addFieldColumn(
    const QString& name,
    const QSharedPointer<QVector<QString>>& values) {
  if (FieldIndex.contains(name)) {
    Records->at(FieldIndex.value(name))->append(*values);
  } else {
    Records->append(values);
    Fields.append(name);
    FieldIndex.insert(name, FieldIndex.size());
  }
}

void SqlRecordCreationForm::addFieldValue(const QString& name,
                                          const QString& value) {
  if (FieldIndex.contains(name)) {
    Records->at(FieldIndex.value(name))->append(value);
  } else {
    QSharedPointer<QVector<QString>> values(new QVector<QString>);
    values->append(value);
    FieldIndex.insert(name, FieldIndex.size());
    Records->append(values);
    Fields.append(name);
  }
}

void SqlRecordCreationForm::clear() {
  Records.clear();
  Fields.clear();
}
