#ifndef SELECTSTREAMDIALOG_COPY1_H
#define SELECTSTREAMDIALOG_COPY1_H

#include <QDialog>
#include <QMessageBox>
#include <QString>
#include "Mk03/engineContainer.h"

namespace Ui {
class selectStreamDialog_copy1;
}

class selectStreamDialog_copy1 : public QDialog {
  Q_OBJECT

 public:
  explicit selectStreamDialog_copy1(Mk03::engineContainer<Mk03::bitrateAnalysis>* aPePtr, QWidget* parent = nullptr);
  ~selectStreamDialog_copy1();

 signals:
  void emitStreamArguments(int a, int v, int64_t seek_ms = -1);

 private slots:
  void on_buttonBox_accepted();

 private:
  Mk03::engineContainer<Mk03::bitrateAnalysis>* pePtr;
  Ui::selectStreamDialog_copy1* ui;
};

#endif  // SELECTSTREAMDIALOG_COPY1_H
