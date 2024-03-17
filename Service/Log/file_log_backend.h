#ifndef FILELOGBACKEND_H
#define FILELOGBACKEND_H

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTextStream>

#include "abstract_log_backend.h"
#include "configurable_object.h"
#include "loggable_object.h"
#include "named_object.h"

class FileLogBackend final : public NamedObject,
                             public AbstractLogBackend,
                             public ConfigurableObject,
                             public LoggableObject {
 private:
  bool Enable;
  QDir CurrentDir;
  QFile CurrentFile;
  QTextStream FileStream;

 public:
  explicit FileLogBackend(const QString& name);
  ~FileLogBackend();

  // AbstractLogBackend interface
 public:
  virtual void writeMessage(const QString& str) override;

 private:
  Q_DISABLE_COPY_MOVE(FileLogBackend)

 private:
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

 private:
  void initialize();
  void removeOldestLogFiles(void);
};

#endif /* FILELOGBACKEND_H */
