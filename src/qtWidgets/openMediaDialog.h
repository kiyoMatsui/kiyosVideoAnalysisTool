#ifndef OPENMEDIADIALOG_H
#define OPENMEDIADIALOG_H

#include <QDialog>

namespace Ui {
class openMediaDialog;
}

class openMediaDialog final : public QDialog {
  Q_OBJECT

  friend class openmediatest;

 public:
  explicit openMediaDialog(QWidget *parent = 0);
  ~openMediaDialog();

 public slots:
  void setMediaSource(const QString& aString);

 signals:
  void returnMediaSource(QString aMediaSource);

 private slots:
  void on_buttonBox_accepted();

  void on_toolButton_clicked();

 private:
  Ui::openMediaDialog *ui;
};

#endif  // OPENMEDIADIALOG_H
