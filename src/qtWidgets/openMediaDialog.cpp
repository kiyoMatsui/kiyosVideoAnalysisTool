#include "openMediaDialog.h"
#include <QFileDialog>
#include "ui_openMediaDialog.h"

openMediaDialog::openMediaDialog(QWidget *parent) : QDialog(parent), ui(new Ui::openMediaDialog) {
  ui->setupUi(this);
}

openMediaDialog::~openMediaDialog() {
  delete ui;
}

void openMediaDialog::on_buttonBox_accepted() {
  emit returnMediaSource(ui->lineEdit->text().trimmed());
  disconnect(this, nullptr, nullptr, nullptr);
}

void openMediaDialog::setMediaSource(const QString& aString) {
  ui->lineEdit->setText(aString);
}

void openMediaDialog::on_toolButton_clicked() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Local file"), ui->lineEdit->text(),
                                                  tr("All Files (*);;mp4 Files (*.mp4)"));
  if (!fileName.isEmpty()) ui->lineEdit->setText(fileName);
}
