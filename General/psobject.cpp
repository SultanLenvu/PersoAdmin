#include "psobject.h"
#include "global_environment.h"
#include "log_system.h"

PSObject::PSObject(const QString& name) : QObject{nullptr} {
  setObjectName(name);
  GlobalEnvironment::instance()->registerObject(this);

  connectDependencies();
}

PSObject::~PSObject() {}

void PSObject::applySettings() {
}

void PSObject::sendLog(const QString& log) const {
  emit logging(QString("%1 - %2").arg(objectName(), log));
}

PSObject::PSObject() {}

void PSObject::connectDependencies() {
  LogSystem* ls = static_cast<LogSystem*>(
      GlobalEnvironment::instance()->getObject("LogSystem"));

  connect(this, &PSObject::logging, ls, &LogSystem::generate);
}
