#include "production_line_gui_subkernel.h"
#include "database_manager.h"
#include "global_environment.h"
#include "production_line_creation_dialog.h"
#include "production_line_manager.h"
#include "string_input_dialog.h"

ProductionLineGuiSubkernel::ProductionLineGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}

ProductionLineGuiSubkernel::~ProductionLineGuiSubkernel() {}

SqlResponseModel* ProductionLineGuiSubkernel::productionLines() const {
  return ProductionLines.get();
}

void ProductionLineGuiSubkernel::create() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  ProductionLineCreationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit create_signal(param);
}

void ProductionLineGuiSubkernel::activate() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

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
  dialog.getData(*param.get());

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
  ProductionLineManager* om = static_cast<ProductionLineManager*>(
      GlobalEnvironment::instance()->getObject("ProductionLineManager"));
  DatabaseManager* dm = static_cast<DatabaseManager*>(
      GlobalEnvironment::instance()->getObject("DatabaseManager"));

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
          &DatabaseManager::getTable);

  // От менеджеров
  connect(dm, &DatabaseManager::responseReady, this,
          &ProductionLineGuiSubkernel::display);
}
