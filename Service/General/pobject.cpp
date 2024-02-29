#include "global_environment.h"
#include "log_system.h"
#include "pobject.h"

PObject::PObject(const QString& name) : QObject{nullptr} {
  setObjectName(name);

  GlobalEnvironment::instance()->registerObject(this);
  connectDependencies();
}

PObject::~PObject() {
  emit deleted(objectName());
}

bool PObject::init() {
  sendLog("Инициализация.");

  Valid = initInternals();

  return Valid;
}

void PObject::sendLog(const QString& log) const {
  emit logging(QString("%1 - %2").arg(objectName(), log));
}

PObject::PObject() {}

bool PObject::initInternals() {
  return true;
}

void PObject::connectDependencies() {
  LogSystem* ls = static_cast<LogSystem*>(
      GlobalEnvironment::instance()->getObject("LogSystem"));

  connect(this, &PObject::logging, ls, &LogSystem::generate);
}
