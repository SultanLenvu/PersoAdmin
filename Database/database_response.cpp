#include "database_response.h"

DatabaseResponse::DatabaseResponse(QObject* parent) : QObject(parent) {}

DatabaseResponse::~DatabaseResponse() {}

void DatabaseResponse::addField(const QString& name,
                                const QSharedPointer<QStringList>) {
  Records.insert(name, QSharedPointer(new QVector));
}

void DatabaseResponse::prepareMemoryForRecords(uint32_t num) {
  for (HashRecordTable::const_iterator it = Records.constBegin();
       it != Records.constEnd(); it++) {
    it.value()->resize(num);
  }
}

uint32_t DatabaseResponse::getRecordNumber() const {
  if (Records.first()->is) {
  }
}

bool DatabaseResponse::isEmpty() {}

QString DatabaseResponse::getValue(uint32_t record, const QString& field) {}
