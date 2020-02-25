#ifndef BITRATEFORM_H
#define BITRATEFORM_H

#include <QDialog>
#include <QTimer>


namespace Ui {
  class bitrateForm;
}

class bitrateForm : public QDialog
{
  Q_OBJECT

public:
  explicit bitrateForm(QWidget *parent = nullptr);
  ~bitrateForm();
  void realtimeDataSlot();

private:
  void paintEvent(QPaintEvent *event) override;

private:
  QTimer dataTimer;
  Ui::bitrateForm *ui;
};

#endif // BITRATEFORM_H
