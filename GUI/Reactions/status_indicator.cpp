#include <QMessageBox>

#include "status_indicator.h"

StatusIndicator::StatusIndicator(const QString& name)
    : NamedObject{name} {
  createMessageMatchTable();
}

void StatusIndicator::handle(ReturnStatus ret) {
  if (ret == ReturnStatus::NoError) {
    QMessageBox::information(nullptr, "Сообщение", MessageTable[ret],
                             QMessageBox::Ok);
    return;
  }
  if (MessageTable.count(ret) == 0) {
    QMessageBox::critical(nullptr, "Ошибка",
                          "Получен неизвестный статус возврата.",
                          QMessageBox::Ok);
    return;
  }

  QMessageBox::critical(nullptr, "Ошибка", MessageTable[ret], QMessageBox::Ok);
}

void StatusIndicator::createMessageMatchTable() {
  MessageTable[ReturnStatus::NoError] = QString("Выполнено. ");

  MessageTable[ReturnStatus::ParameterError] =
      QString("Получены некорректные параметры.");
  MessageTable[ReturnStatus::DynamicLibraryMissing] =
      QString("Отсутствует библиотека для работы с принтером стикеров. ");
  MessageTable[ReturnStatus::StickerPrinterConnectionError] =
      QString("Не удалось подключиться к стикер-принтеру. ");
  MessageTable[ReturnStatus::StickerPrintingFailed] =
      QString("Не удалось распечать стикер. ");

  MessageTable[ReturnStatus::DatabaseConnectionError] =
      QString("Не удалось подключиться к базе данных. ");
  MessageTable[ReturnStatus::DatabaseTransactionError] =
      QString("Ошибка транзакции. ");
  MessageTable[ReturnStatus::DatabaseQueryError] =
      QString("Получена ошибка при выполнении запроса к базе данных.");

  MessageTable[ReturnStatus::RegisterFileError] =
      QString("Не удалось открыть файл-реестр для отгрузки.");

  MessageTable[ReturnStatus::TransponderMissed] =
      QString("Транспондер не найден.");
  MessageTable[ReturnStatus::BoxMissed] = QString("Бокс не найден.");
  MessageTable[ReturnStatus::PalletMissed] = QString("Паллета не найдена.");
  MessageTable[ReturnStatus::OrderMissed] = QString("Заказ не найден.");

  MessageTable[ReturnStatus::ProductionLineMissed] =
      QString("Не удалось найти производственную линию.");
  MessageTable[ReturnStatus::ProductionLineLinkError] =
      QString("Не удалось связать производственную линию с боксом.");
  MessageTable[ReturnStatus::ProductionLineRollbackLimit] = QString(
      "Производственная линия находится на первом транспондере в боксе.");

  MessageTable[ReturnStatus::OrderRemovingError] =
      QString("Не удалось удалить заказ.");
  MessageTable[ReturnStatus::OtherOrderInProcess] =
      QString("В процессе сборки находится другой заказ.");
  MessageTable[ReturnStatus::OrderNotInProcess] =
      QString("Сборка заказа не была запущена.");

  MessageTable[ReturnStatus::OrderMultiplyAssembly] =
      QString("В процессе сборки находится несколько заказов.");
  MessageTable[ReturnStatus::FreeBoxMissed] =
      QString("Не удалось найти свободный бокс.");
  MessageTable[ReturnStatus::UnknownError] = QString("Неизвествная ошибка. ");
}
