#include "asynchronous_object_space.h"

#include "database_async_wrapper.h"
#include "named_object_factory.h"
#include "order_manager_async_wrapper.h"
#include "perso_server_async_wrapper.h"
#include "production_line_manager_async_wrapper.h"
#include "programmer_async_wrapper.h"
#include "sticker_printer_async_wrapper.h"

AsynchronousObjectSpace::AsynchronousObjectSpace() {
  Thread.start();
  createWrappers();
}

AsynchronousObjectSpace::~AsynchronousObjectSpace() {
  Thread.quit();
  Thread.wait();
}

void AsynchronousObjectSpace::createWrappers() {
  NamedObjectFactory factory(&Thread);

  std::unique_ptr<DatabaseAsyncWrapper> daw(
      factory.create<DatabaseAsyncWrapper>("DatabaseAsyncWrapper"));

  Managers.emplace_back(factory.create<OrderManagerAsyncWrapper>(
      "OrderManagerAsyncWrapper", daw->database()));
  Managers.emplace_back(factory.create<ProductionLineManagerAsyncWrapper>(
      "ProductionLineManagerAsyncWrapper", daw->database()));
  Managers.emplace_back(std::move(daw));
  Managers.emplace_back(
      factory.create<PersoServerAsyncWrapper>("PersoServerAsyncWrapper"));
  Managers.emplace_back(
      factory.create<ProgrammerAsyncWrapper>("ProgrammerAsyncWrapper"));
  Managers.emplace_back(
      factory.create<StickerPrinterAsyncWrapper>("StickerPrinterAsyncWrapper"));
}
