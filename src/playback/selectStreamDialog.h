#ifndef SELECTSTREAMDIALOG_H
#define SELECTSTREAMDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QString>
#include "Mk03/engineContainer.h"

namespace Ui {
class selectStreamDialog;
}

class selectStreamDialog final : public QDialog {
  Q_OBJECT

 public:
  explicit selectStreamDialog(Mk03::engineContainer<> *aPePtr, QWidget *parent = nullptr);
  ~selectStreamDialog();

 signals:
  void emitStreamArguments(int a, int v, int64_t seek_ms = -1);

 private slots:
  void on_buttonBox_accepted();

 private:
  Mk03::engineContainer<> *pePtr;
  Ui::selectStreamDialog *ui;
};

#endif  // SELECTSTREAMDIALOG_H
