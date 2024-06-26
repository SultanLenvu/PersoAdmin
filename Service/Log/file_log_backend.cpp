#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

#include "file_log_backend.h"

FileLogBackend::FileLogBackend(const QString& name)
    : NamedObject(name), LoggableObject(name) {
  doLoadSettings();
  initialize();
}

FileLogBackend::~FileLogBackend() {
  CurrentFile.close();
}

void FileLogBackend::writeMessage(const QString& str) {
  if (!Enable) {
    return;
  }

  FileStream << str << "\n";
}

void FileLogBackend::loadSettings() {
  sendLog("Загрузка настроек.");
  doLoadSettings();
  initialize();
}

void FileLogBackend::doLoadSettings() {
  QSettings settings;

  Enable = settings.value("log_system/file_log_enable").toBool();
  CurrentDir = settings.value("log_system/file_directory").toString();
}

void FileLogBackend::initialize() {
  CurrentFile.close();

  QFileInfo info(CurrentDir.path());
  if (!info.isDir() || !info.isWritable() || !info.isReadable()) {
    Enable = false;
    QMessageBox::critical(nullptr, "Ошибка",
                          "Некоррректная директория для логгирования. ",
                          QMessageBox::Ok);
    return;
  }

  // !!! Опасно !!!
  //  removeOldestLogFiles();

  CurrentFile.setFileName(
      CurrentDir.path() +
      QDateTime::currentDateTime().toString("/log-dd.MM.yyyy-hh.mm.ss"));
  if (!CurrentFile.open(QIODevice::WriteOnly)) {
    Enable = false;
    QMessageBox::critical(nullptr, "Ошибка",
                          "Не удалось открыть файл для логгирования. ",
                          QMessageBox::Ok);
    return;
  }

  FileStream.setDevice(&CurrentFile);
}

void FileLogBackend::removeOldestLogFiles() {
  // Получаем список файлов в директории
  //  QFileInfoList fileList = CurrentDir.entryInfoList(
  //      QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);

  //  int32_t fileCount = fileList.size();

  //  if (fileCount > FileMaxNumber) {
  //    int32_t filesToDeleteCount = fileCount - FileMaxNumber;

  //    // Удаляем самые старые файлы
  //    for (int32_t i = 0; i < filesToDeleteCount; ++i) {
  //      const QFileInfo& fileInfo = fileList.at(i);
  //      QString filePath = fileInfo.absoluteFilePath();

  //      QFile::remove(filePath);
  //    }
  //  }
}
