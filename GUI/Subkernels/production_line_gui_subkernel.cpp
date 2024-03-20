#include "production_line_gui_subkernel.h"
#include "database_async_wrapper.h"
#include "global_environment.h"
#include "production_line_creation_dialog.h"
#include "production_line_manager_async_wrapper.h"
#include "string_input_dialog.h"

ProductionLineGuiSubkernel::ProductionLineGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name), ProductionLines(new SqlResponseModel()) {
  connectDependecies();
}

ProductionLineGuiSubkernel::~ProductionLineGuiSubkernel() {}

SqlResponseModel* ProductionLineGuiSubkernel::productionLines() {
  return ProductionLines.get();
}

void ProductionLineGuiSubkernel::create() {
  StringDictionary& param(new StringDictionary);

  ProductionLineCreationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit create_signal(param);
}

void ProductionLineGuiSubkernel::activate() {
  StringDictionary& param(new StringDictionary);

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit activate_signal(param);
}

void ProductionLineGuiSubkernel::activateAll() {
  emit clearLogDisplay();
  emit activateAll_signal();
}

void ProductionLineGuiSubkernel::deactivate() {
  StringDictionary& param(new StringDictionary);

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit deactivate_signal(param);
}

void ProductionLineGuiSubkernel::deactivateAll() {
  emit clearLogDisplay();
  emit deactivateAll_signal();
}

void ProductionLineGuiSubkernel::edit() {
  // TODO

  emit clearLogDisplay();
}

void ProductionLineGuiSubkernel::get() {
  emit clearLogDisplay();
  emit get_signal("production_lines");
}

void ProductionLineGuiSubkernel::displayResponse(
    std::shared_ptr<SqlQueryValues> data) {
  ProductionLines->setResponse(data);
}

void ProductionLineGuiSubkernel::connectDependecies() {
  ProductionLineManagerAsyncWrapper* om =
      GlobalEnvironment::instance()
          ->getObject<ProductionLineManagerAsyncWrapper>(
              "ProductionLineManagerAsyncWrapper");
  const DatabaseAsyncWrapper* dm =
      GlobalEnvironment::instance()->getObject<DatabaseAsyncWrapper>(
          "DatabaseAsyncWrapper");

  // К менеджерам
  connect(this, &ProductionLineGuiSubkernel::create_signal, om,
          &ProductionLineManagerAsyncWrapper::create);
  connect(this, &ProductionLineGuiSubkernel::activate_signal, om,
          &ProductionLineManagerAsyncWrapper::activate);
  connect(this, &ProductionLineGuiSubkernel::activateAll_signal, om,
          &ProductionLineManagerAsyncWrapper::activateAll);
  connect(this, &ProductionLineGuiSubkernel::deactivate_signal, om,
          &ProductionLineManagerAsyncWrapper::deactivate);
  connect(this, &ProductionLineGuiSubkernel::deactivateAll_signal, om,
          &ProductionLineManagerAsyncWrapper::deactivateAll);
  connect(this, &ProductionLineGuiSubkernel::edit_signal, om,
          &ProductionLineManagerAsyncWrapper::edit);

  connect(this, &ProductionLineGuiSubkernel::get_signal, dm,
          &DatabaseAsyncWrapper::getTable);
}
