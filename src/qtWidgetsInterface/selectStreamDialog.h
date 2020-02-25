#ifndef SELECTSTREAMDIALOG_H
#define SELECTSTREAMDIALOG_H


#include <QDialog>
#include <QString>
#include <QMessageBox>
#include "Mk02/engineContainer.h"

namespace Ui {
  class selectStreamDialog;
}

class selectStreamDialog : public QDialog
{
  Q_OBJECT

public:
  explicit selectStreamDialog(Mk02::engineContainer* aPePtr, QWidget *parent = nullptr);
  ~selectStreamDialog();

signals:
  void emitStreamArguments(int a, int v, int64_t seek_ms = -1);

private slots:
  void on_buttonBox_accepted();

private:
  Mk02::engineContainer* pePtr;
  Ui::selectStreamDialog *ui;
};

#endif // SELECTSTREAMDIALOG_H
