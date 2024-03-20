#include "order_gui_subkernel.h"

#include "database_async_wrapper.h"
#include "global_environment.h"
#include "link_issuer_key_dialog.h"
#include "manual_release_refund_dialog.h"
#include "order_creation_dialog.h"
#include "order_manager_async_wrapper.h"
#include "pallet_shiping_dialog.h"
#include "string_input_dialog.h"

OrderGuiSubkernel::OrderGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}

SqlResponseModel& OrderGuiSubkernel::orders() {
  return Orders;
}

void OrderGuiSubkernel::create() {
  StringDictionary param;

  OrderCreationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit create_signal(param);
}

void OrderGuiSubkernel::startAssembling() {
  StringDictionary param;

  StringInputDialog dialog("id");

  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit startAssembling_signal(param);
}

void OrderGuiSubkernel::stopAssembling() {
  StringDictionary param;

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit stopAssembling_signal(param);
}

void OrderGuiSubkernel::get() {
  emit clearLogDisplay();
  emit get_signal("orders");
}

void OrderGuiSubkernel::release() {
  StringDictionary param;

  ManualReleaseRefundDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit release_signal(param);
}

void OrderGuiSubkernel::refund() {
  StringDictionary param;

  ManualReleaseRefundDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit refund_signal(param);
}

void OrderGuiSubkernel::generateShipmentRegister() {
  StringDictionary param;

  PalletShippingDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

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
  StringDictionary param;

  LinkIssuerKeyDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit linkIssuerWithKeys_signal(param);
}

void OrderGuiSubkernel::displayResponse(
    std::shared_ptr<SqlQueryValues> orders) {
  Orders.setResponse(orders);
}

void OrderGuiSubkernel::connectDependecies() {
  const OrderManagerAsyncWrapper* om =
      GlobalEnvironment::instance()->getObject<const OrderManagerAsyncWrapper>(
          "OrderManagerAsyncWrapper");
  const DatabaseAsyncWrapper* dm =
      GlobalEnvironment::instance()->getObject<const DatabaseAsyncWrapper>(
          "DatabaseAsyncWrapper");

  // К менеджерам
  connect(this, &OrderGuiSubkernel::create_signal, om,
          &OrderManagerAsyncWrapper::create);
  connect(this, &OrderGuiSubkernel::startAssembling_signal, om,
          &OrderManagerAsyncWrapper::startAssembling);
  connect(this, &OrderGuiSubkernel::stopAssembling_signal, om,
          &OrderManagerAsyncWrapper::stopAssembling);
  connect(this, &OrderGuiSubkernel::release_signal, om,
          &OrderManagerAsyncWrapper::release);
  connect(this, &OrderGuiSubkernel::refund_signal, om,
          &OrderManagerAsyncWrapper::refund);
  connect(this, &OrderGuiSubkernel::shipPallets_signal, om,
          &OrderManagerAsyncWrapper::generateShipmentRegister);
  connect(this, &OrderGuiSubkernel::initIssuers_signal, om,
          &OrderManagerAsyncWrapper::initIssuers);
  connect(this, &OrderGuiSubkernel::initTransportMasterKeys_signal, om,
          &OrderManagerAsyncWrapper::initTransportMasterKeys);
  connect(this, &OrderGuiSubkernel::linkIssuerWithKeys_signal, om,
          &OrderManagerAsyncWrapper::linkIssuerWithKeys);

  connect(this, &OrderGuiSubkernel::get_signal, dm, &DatabaseAsyncWrapper::getTable);
}
