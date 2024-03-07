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

class FileLogBackend : public AbstractLogBackend, public ConfigurableObject {
 private:
  bool Enable;
  QDir CurrentDir;
  QFile CurrentFile;
  QTextStream FileStream;

 public:
  explicit FileLogBackend();
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
