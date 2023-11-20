#include "order_creation_menu.h"

OrderCreationMenu::OrderCreationMenu(QWidget* parent)
    : AbstractInputDialog(parent) {}

OrderCreationMenu::~OrderCreationMenu() {}

void OrderCreationMenu::getData(QHash<QString, QString>* data) const {}

AbstractInputDialog::DialogType OrderCreationMenu::type() const {
  return OrderCreation;
}

void OrderCreationMenu::create() {}

bool OrderCreationMenu::check() const {
  return true;
}
