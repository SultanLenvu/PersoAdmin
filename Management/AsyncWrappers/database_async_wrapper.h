#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "i_sql_database.h"
#include "progressable_async_wrapper.h"
#include "types.h"

class DatabaseAsyncWrapper final : public ProgressableAsyncWrapper {
  Q_OBJECT
 private:
  std::shared_ptr<ISqlDatabase> Database;

 public:
  Q_INVOKABLE explicit DatabaseAsyncWrapper(const QString& name);
  ~DatabaseAsyncWrapper() = default;

 public:
  std::shared_ptr<ISqlDatabase> database(void);

 public slots:
  void connect(void);
  void disconnect(void);
  void getTable(const QString& name);
  void execCustomRequest(const QString& req);

  void getTransponderData(const StringDictionary& param);
  void getBoxData(const StringDictionary& param);
  void getPalletData(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(DatabaseAsyncWrapper)

  bool generateTransponderData(const QString& id, StringDictionary& data);
  bool generateBoxData(const QString& id, StringDictionary& data);
  bool generatePalletData(const QString& id, StringDictionary& data);

 signals:
  void dataReady(const StringDictionary& data);
  void responseReady(const std::shared_ptr<SqlQueryValues> response);
};

#endif  // DATABASEMANAGER_H
