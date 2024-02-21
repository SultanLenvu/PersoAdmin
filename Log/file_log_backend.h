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
  int32_t FileMaxNumber;
  QDir CurrentDir;
  QFile CurrentFile;
  QTextStream FileStream;

 public:
  explicit FileLogBackend(const QString& name);
  ~FileLogBackend();

  // PObject interface
 public:
  virtual void applySettings(void) override;

  // LogBackend interface
 public:
  virtual void writeLogMessage(const QString& str) override;

 private:
  Q_DISABLE_COPY(FileLogBackend);
  void loadSettings(void);
  void initialize();
  void removeOldestLogFiles(void);
};

#endif /* FILELOGBACKEND_H */
