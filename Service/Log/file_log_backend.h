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

#include "log_backend.h"

class FileLogBackend : public LogBackend {
  Q_OBJECT
 private:
  bool Enable;
  QDir CurrentDir;
  QFile CurrentFile;
  QTextStream FileStream;

 public:
  explicit FileLogBackend(const QString& name);
  ~FileLogBackend();

  // LogBackend interface
 public:
  virtual void writeMessage(const QString& str) override;

 private:
  Q_DISABLE_COPY_MOVE(FileLogBackend)
  virtual bool initInternals(void) override;
  virtual void loadSettings(void) override;

  void doLoadSettings(void);

  void initialize();
  void removeOldestLogFiles(void);
};

#endif /* FILELOGBACKEND_H */
