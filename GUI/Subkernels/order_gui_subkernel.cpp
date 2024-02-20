#include "order_gui_subkernel.h"

#include "database_manager.h"
#include "global_environment.h"
#include "link_issuer_key_dialog.h"
#include "manual_release_refund_dialog.h"
#include "order_creation_dialog.h"
#include "order_manager.h"
#include "pallet_shiping_dialog.h"
#include "string_input_dialog.h"

OrderGuiSubkernel::OrderGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}

OrderGuiSubkernel::~OrderGuiSubkernel() {}

SqlResponseModel& OrderGuiSubkernel::orders() {
  return Orders;
}

void OrderGuiSubkernel::create() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  OrderCreationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit create_signal(param);
}

void OrderGuiSubkernel::startAssembling() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog;

  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit startAssembling_signal(param);
}

void OrderGuiSubkernel::stopAssembling() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit stopAssembling_signal(param);
}

void OrderGuiSubkernel::get() {
  emit clearLogDisplay();
  emit get_signal("orders");
}

void OrderGuiSubkernel::release() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  ManualReleaseRefundDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit release_signal(param);
}

void OrderGuiSubkernel::refund() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  ManualReleaseRefundDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit refund_signal(param);
}

void OrderGuiSubkernel::generateShipmentRegister() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  PalletShippingDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit shipPallets_signal(param);
}

void OrderGuiSubkernel::initTransportMasterKeys() {
  emit clearLogDisplay();
  emit initTransportMasterKeys_signal();
}

void OrderGuiSubkernel::initIssuers() {
  emit clearLogDisplay();
  emit initIssuers_signal();
}

void OrderGuiSubkernel::linkIssuerWithKeys() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  LinkIssuerKeyDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit linkIssuerWithKeys_signal(param);
}

void OrderGuiSubkernel::display(std::shared_ptr<SqlQueryValues> orders) {
  Orders.setResponse(orders);
}

void OrderGuiSubkernel::connectDependecies() {
  const OrderManager* om = static_cast<const OrderManager*>(
      GlobalEnvironment::instance()->getObject("OrderManager"));
  const DatabaseManager* dm = static_cast<const DatabaseManager*>(
      GlobalEnvironment::instance()->getObject("DatabaseManager"));

  // К менеджерам
  connect(this, &OrderGuiSubkernel::create_signal, om, &OrderManager::create);
  connect(this, &OrderGuiSubkernel::startAssembling_signal, om,
          &OrderManager::startAssembling);
  connect(this, &OrderGuiSubkernel::stopAssembling_signal, om,
          &OrderManager::stopAssembling);
  connect(this, &OrderGuiSubkernel::release_signal, om, &OrderManager::release);
  connect(this, &OrderGuiSubkernel::refund_signal, om, &OrderManager::refund);
  connect(this, &OrderGuiSubkernel::shipPallets_signal, om,
          &OrderManager::generateShipmentRegister);
  connect(this, &OrderGuiSubkernel::initIssuers_signal, om,
          &OrderManager::initIssuers);
  connect(this, &OrderGuiSubkernel::initTransportMasterKeys_signal, om,
          &OrderManager::initTransportMasterKeys);
  connect(this, &OrderGuiSubkernel::linkIssuerWithKeys_signal, om,
          &OrderManager::linkIssuerWithKeys);

  connect(this, &OrderGuiSubkernel::get_signal, dm, &DatabaseManager::getTable);

  // От менеджеров
  connect(dm, &DatabaseManager::responseReady, this,
          &OrderGuiSubkernel::display);
}
