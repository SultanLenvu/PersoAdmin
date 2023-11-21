#include "order_creation_menu.h"
#include "General/definitions.h"

OrderCreationMenu::OrderCreationMenu(QWidget* parent)
    : AbstractInputDialog(parent) {
  setObjectName("OrderCreationMenu");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.45, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);
  setWindowTitle("Создание нового заказа");

  create();
}

OrderCreationMenu::~OrderCreationMenu() {}

void OrderCreationMenu::getData(QHash<QString, QString>* data, bool& ok) const {
  if (!check()) {
    ok = false;
    return;
  }

  data->insert("issuer_name", IssuerNameComboBox->currentText());
  data->insert("transponder_quantity", TransponderQuantityLineEdit->text());
  data->insert("box_capacity", BoxCapacityLineEdit->text());
  data->insert("pallet_capacity", PalletCapacityLineEdit->text());
  data->insert("full_personalization",
               FullPersonalizationCheckBox->checkState() == Qt::Checked
                   ? "true"
                   : "false");
  data->insert("pan_file_path", PanFilePathLineEdit->text());
  data->insert("transponder_model", TransponderModelLineEdit->text());
  data->insert("accr_reference", AccrReferenceLineEdit->text());
  data->insert("equipment_class", EquipmentClassLineEdit->text());
  data->insert("manufacturer_id", ManufacturerIdLineEdit->text());

  ok = true;
}

AbstractInputDialog::DialogType OrderCreationMenu::type() const {
  return OrderCreation;
}

void OrderCreationMenu::create() {
  FullPersonalizationCheckBox = new QCheckBox("Полная персонализация");
  MainLayout->addWidget(FullPersonalizationCheckBox, 0, 0, 1, 3);

  PanFilePathLabel = new QLabel("PAN-файл");
  MainLayout->addWidget(PanFilePathLabel, 1, 0, 1, 1);
  PanFilePathLineEdit = new QLineEdit();
  MainLayout->addWidget(PanFilePathLineEdit, 1, 1, 1, 1);
  PanFileExplorePushButton = new QPushButton("Обзор");
  MainLayout->addWidget(PanFileExplorePushButton, 1, 2, 1, 1);
  connect(PanFileExplorePushButton, &QPushButton::clicked, this,
          &OrderCreationMenu::panFileExplore_slot);

  IssuerNameComboLabel = new QLabel("Компания заказчик");
  MainLayout->addWidget(IssuerNameComboLabel, 2, 0, 1, 1);
  IssuerNameComboBox = new QComboBox();
  IssuerNameComboBox->addItem("Новое качество дорог");
  IssuerNameComboBox->addItem("Западный скоростной диаметр");
  MainLayout->addWidget(IssuerNameComboBox, 2, 1, 1, 2);

  TransponderQuantityLabel = new QLabel("Количество транспондеров");
  MainLayout->addWidget(TransponderQuantityLabel, 3, 0, 1, 1);
  TransponderQuantityLineEdit = new QLineEdit("500");
  MainLayout->addWidget(TransponderQuantityLineEdit, 3, 1, 1, 2);

  BoxCapacityLabel = new QLabel("Емкость бокса");
  MainLayout->addWidget(BoxCapacityLabel, 4, 0, 1, 1);
  BoxCapacityLineEdit = new QLineEdit("50");
  MainLayout->addWidget(BoxCapacityLineEdit, 4, 1, 1, 2);

  PalletCapacityLabel = new QLabel("Емкость палеты");
  MainLayout->addWidget(PalletCapacityLabel, 5, 0, 1, 1);
  PalletCapacityLineEdit = new QLineEdit("10");
  MainLayout->addWidget(PalletCapacityLineEdit, 5, 1, 1, 2);

  TransponderModelLabel = new QLabel("Модель транспондера");
  MainLayout->addWidget(TransponderModelLabel, 6, 0, 1, 1);
  TransponderModelLineEdit = new QLineEdit("PS1001");
  MainLayout->addWidget(TransponderModelLineEdit, 6, 1, 1, 2);

  AccrReferenceLabel = new QLabel("ACCR Reference (HEX)");
  MainLayout->addWidget(AccrReferenceLabel, 7, 0, 1, 1);
  AccrReferenceLineEdit = new QLineEdit("1DD1");
  MainLayout->addWidget(AccrReferenceLineEdit, 7, 1, 1, 2);

  EquipmentClassLabel = new QLabel("Класс оборудования (HEX)");
  MainLayout->addWidget(EquipmentClassLabel, 8, 0, 1, 1);
  EquipmentClassLineEdit = new QLineEdit("F301");
  MainLayout->addWidget(EquipmentClassLineEdit, 8, 1, 1, 2);

  ManufacturerIdLabel = new QLabel("Идентификатор производителя (HEX)");
  MainLayout->addWidget(ManufacturerIdLabel, 9, 0, 1, 1);
  ManufacturerIdLineEdit = new QLineEdit("0032");
  MainLayout->addWidget(ManufacturerIdLineEdit, 9, 1, 1, 2);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout, 10, 0, 1, 3);

  AcceptButton = new QPushButton("Ввод");
  MainLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool OrderCreationMenu::check() const {
  int32_t transponderQuantity = TransponderQuantityLineEdit->text().toInt();
  int32_t boxCapacity = BoxCapacityLineEdit->text().toInt();
  int32_t palletCapacity = PalletCapacityLineEdit->text().toInt();
  QString transponderModel = TransponderModelLineEdit->text();
  QString accrReference = AccrReferenceLineEdit->text();
  QString equipmnetClass = EquipmentClassLineEdit->text();
  QString manufacturerId = ManufacturerIdLineEdit->text();
  QString panFilePath = PanFilePathLineEdit->text();

  if (transponderQuantity <= 0) {
    return false;
  }

  if (boxCapacity <= 0) {
    return false;
  }

  if (palletCapacity <= 0) {
    return false;
  }

  if ((transponderQuantity % (boxCapacity * palletCapacity)) != 0) {
    return false;
  }

  if ((transponderModel.length() > TRANSPONDER_MODEL_CHAR_LENGTH) ||
      (transponderModel.length() == 0)) {
    return false;
  }

  QFileInfo info(PanFilePathLineEdit->text());
  if ((!info.exists()) || (!info.isFile()) || (info.suffix() != "csv")) {
    return false;
  }

  QFile file(panFilePath);
  int32_t recordCount = 0;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString record = in.readLine();
    if (record.length() != PAN_CHAR_LENGTH) {
      file.close();
      return false;
    }
    recordCount++;
  }
  file.close();

  if (recordCount != transponderQuantity) {
    return false;
  }

  if ((accrReference.length() > ACCR_REFERENCE_CHAR_LENGTH) ||
      (accrReference.length() == 0)) {
    return false;
  }

  if ((equipmnetClass.length() > EQUIPMENT_CLASS_CHAR_LENGTH) ||
      (equipmnetClass.length() == 0)) {
    return false;
  }

  if ((manufacturerId.length() > MANUFACTURER_ID_CHAR_LENGTH) ||
      (manufacturerId.length() == 0)) {
    return false;
  }

  return true;
}

void OrderCreationMenu::panFileExplore_slot() {
  QString filePath =
      QFileDialog::getOpenFileName(nullptr, "Выбрать файл", "./", "*.csv");
  if (!filePath.isEmpty()) {
    PanFilePathLineEdit->setText(filePath);
  }
}
