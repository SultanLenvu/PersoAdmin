#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "abstract_sql_database.h"
#include "loggable_object.h"
#include "named_object.h"
#include "progressable_async_wrapper.h"
#include "types.h"

class DatabaseAsyncWrapper final : public NamedObject,
                                   public ProgressableAsyncWrapper,
                                   public LoggableObject {
  Q_OBJECT
 private:
  std::shared_ptr<AbstractSqlDatabase> Database;

 public:
  Q_INVOKABLE explicit DatabaseAsyncWrapper(const QString& name);
  ~DatabaseAsyncWrapper();

 public:
  std::shared_ptr<AbstractSqlDatabase> database(void);

 public slots:
  void connect(void);
  void disconnect(void);
  void getTable(const QString& name);
  void execCustomRequest(const QString& req);

  void getTransponderData(const std::shared_ptr<StringDictionary> param);
  void getBoxData(const std::shared_ptr<StringDictionary> param);
  void getPalletData(const std::shared_ptr<StringDictionary> param);

 private:
  Q_DISABLE_COPY_MOVE(DatabaseAsyncWrapper)

  bool generateTransponderData(const QString& id, StringDictionary& data);
  bool generateBoxData(const QString& id, StringDictionary& data);
  bool generatePalletData(const QString& id, StringDictionary& data);

 signals:
  void dataReady(const std::shared_ptr<StringDictionary> data);
  void responseReady(const std::shared_ptr<SqlQueryValues> response);
};

#endif  // DATABASEMANAGER_H
