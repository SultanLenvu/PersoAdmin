#include "abstract_service_space.h"

// Debug
#include <QApplication>

AbstractServiceSpace::AbstractServiceSpace(const QString& name)
    : QObject{nullptr} {
  setObjectName(name);
}

AbstractServiceSpace::~AbstractServiceSpace() {}
