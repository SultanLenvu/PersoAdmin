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
  int32_t LogFileMaxNumber;
  QString CurrentLogDir;
  QFile CurrentLogFile;
  QTextStream LogTextStream;

 public:
  explicit FileLogBackend(const QString& name);
  ~FileLogBackend();

  virtual void writeLogLine(const QString& str) override;
  virtual void clear() override;
  virtual void applySettings() override;

 private:
  Q_DISABLE_COPY_MOVE(FileLogBackend);
  void loadSettings(void);
  void initialize();
  void removeOldestLogFiles(void);
};

#endif /* FILELOGBACKEND_H */
