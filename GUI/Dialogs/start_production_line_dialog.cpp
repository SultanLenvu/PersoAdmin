#include "start_production_line_dialog.h"
#include "General/definitions.h"

StartProductionLineDialog::StartProductionLineDialog(QWidget* parent)
    : AbstractInputDialog(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.45, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);
  setWindowTitle("Запуск производственной линии");

  create();

  adjustSize();
  setFixedHeight(size().height());
}

StartProductionLineDialog::~StartProductionLineDialog() {}

void StartProductionLineDialog::getData(QHash<QString, QString>* data,
                                        bool& ok) const {
  if (!check()) {
    ok = false;
    return;
  }

  data->insert("production_line_id", ProductionLineIdInput->text());
  data->insert("order_id", OrderIdInput->text());

  ok = true;
}

AbstractInputDialog::DialogType StartProductionLineDialog::type() const {
  return StartProductionLine;
}

void StartProductionLineDialog::create() {
  MainLayout = new QGridLayout();
  setLayout(MainLayout);

  ProductionLineIdLabel = new QLabel("Имя сборщика: ");
  MainLayout->addWidget(ProductionLineIdLabel, 0, 0, 1, 1);
  ProductionLineIdInput = new QLineEdit();
  ProductionLineIdInput->setMaxLength(ID_MAX_LENGHT);
  MainLayout->addWidget(ProductionLineIdInput, 0, 1, 1, 1);

  OrderIdLabel = new QLabel("Имя сборщика: ");
  MainLayout->addWidget(OrderIdLabel, 1, 0, 1, 1);
  OrderIdInput = new QLineEdit();
  OrderIdInput->setMaxLength(ID_MAX_LENGHT);
  MainLayout->addWidget(OrderIdInput, 1, 1, 1, 1);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout, 2, 0, 1, 2);

  AcceptButton = new QPushButton("Ввод");
  MainLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool StartProductionLineDialog::check() const {
  if (ProductionLineIdInput->text().toUInt() == 0) {
    return false;
  }
  if (OrderIdInput->text().toUInt() == 0) {
    return false;
  }

  return true;
}
