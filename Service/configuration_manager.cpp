#include "configuration_manager.h"

#include <QCoreApplication>
#include <QSettings>

#include "definitions.h"
#include "types.h"

ConfigurationSystem::ConfigurationSystem(const QString& name)
    : NamedObject(name) {
  init();
  registerMetaTypes();
}

void ConfigurationSystem::init() {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(PROGRAM_NAME);

  QSettings::setDefaultFormat(QSettings::IniFormat);
}

void ConfigurationSystem::registerMetaTypes() {
  qRegisterMetaType<StringDictionary>("StringDictionary");
  qRegisterMetaType<QStringList>("QStringList");
  qRegisterMetaType<ReturnStatus>("ReturnStatus");
  qRegisterMetaType<std::shared_ptr<QByteArray>>("std::shared_ptr<QByteArray>");
}
