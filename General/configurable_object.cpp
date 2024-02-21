#include "configurable_object.h"

ConfigurableObject::ConfigurableObject(const QString& name) : PObject{name} {}

ConfigurableObject::~ConfigurableObject() {}

void ConfigurableObject::applySettings() {
  sendLog("Применение новых настроек.");
  loadSettings();
}

void ConfigurableObject::loadSettings() {}
