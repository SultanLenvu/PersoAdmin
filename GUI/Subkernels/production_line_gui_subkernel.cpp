#include "production_line_gui_subkernel.h"
#include "database_manager.h"
#include "global_environment.h"
#include "production_line_creation_dialog.h"
#include "production_line_manager.h"
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
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  ProductionLineCreationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit create_signal(param);
}

void ProductionLineGuiSubkernel::activate() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog;
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
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog;
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

void ProductionLineGuiSubkernel::display(std::shared_ptr<SqlQueryValues> data) {
  ProductionLines->setResponse(data);
}

void ProductionLineGuiSubkernel::connectDependecies() {
  const ProductionLineManager* om = static_cast<const ProductionLineManager*>(
      GlobalEnvironment::instance()->getObject("ProductionLineManager"));
  const DatabaseAsyncWrapper* dm = static_cast<const DatabaseAsyncWrapper*>(
      GlobalEnvironment::instance()->getObject("DatabaseAsyncWrapper"));

  // К менеджерам
  connect(this, &ProductionLineGuiSubkernel::create_signal, om,
          &ProductionLineManager::create);
  connect(this, &ProductionLineGuiSubkernel::activate_signal, om,
          &ProductionLineManager::activate);
  connect(this, &ProductionLineGuiSubkernel::activateAll_signal, om,
          &ProductionLineManager::activateAll);
  connect(this, &ProductionLineGuiSubkernel::deactivate_signal, om,
          &ProductionLineManager::deactivate);
  connect(this, &ProductionLineGuiSubkernel::deactivateAll_signal, om,
          &ProductionLineManager::deactivateAll);
  connect(this, &ProductionLineGuiSubkernel::edit_signal, om,
          &ProductionLineManager::edit);

  connect(this, &ProductionLineGuiSubkernel::get_signal, dm,
          &DatabaseAsyncWrapper::getTable);

  // От менеджеров
  connect(dm, &DatabaseAsyncWrapper::responseReady, this,
          &ProductionLineGuiSubkernel::display);
}
