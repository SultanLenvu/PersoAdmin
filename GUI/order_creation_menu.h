#ifndef ORDERCREATIONMENU_H
#define ORDERCREATIONMENU_H

#include "abstract_input_dialog.h"

class OrderCreationMenu : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

 public:
  explicit OrderCreationMenu(QWidget* parent);
  ~OrderCreationMenu();

  virtual void getData(QHash<QString, QString>* data) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(OrderCreationMenu);
  void create(void);
  bool check(void) const;
};

#endif  // ORDERCREATIONMENU_H
