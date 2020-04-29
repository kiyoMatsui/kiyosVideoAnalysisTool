#include "selectStreamDialog.h"
#include "ui_selectStreamDialog.h"

selectStreamDialog::selectStreamDialog(Mk03::engineContainer<>* aPePtr, QWidget* parent)
    : QDialog(parent), pePtr(aPePtr), ui(new Ui::selectStreamDialog) {
  ui->setupUi(this);
  ui->audioSpinBox->setRange(-1, pePtr->getStreams() - 1);
  ui->videoSpinBox->setRange(0, pePtr->getStreams() - 1);
  ui->audioSpinBox->setValue(pePtr->getAudioStreamIndex());
  ui->videoSpinBox->setValue(pePtr->getVideoStreamIndex());
}

selectStreamDialog::~selectStreamDialog() {
  delete ui;
}

void selectStreamDialog::on_buttonBox_accepted() {
  if ((pePtr->checkStreamType(ui->audioSpinBox->value()) == 1 ||    // AVMEDIA_TYPE_AUDIO = 1
       pePtr->checkStreamType(ui->audioSpinBox->value()) == -1) &&  // no audio  = -1
      pePtr->checkStreamType(ui->videoSpinBox->value()) == 0) {     // AVMEDIA_TYPE_VIDEO = 0
    emit emitStreamArguments(ui->audioSpinBox->value(), ui->videoSpinBox->value());
  } else {
    QMessageBox::warning(this, ("stream index error"),
                         ("<p>Stream index wrong type! "
                          "<p>Hit Metadata to check which stream (index) is a video/audio type. "
                          "<p>Streams set to default."),
                         QMessageBox::Abort);
    emit emitStreamArguments(-42, -42);  // magic number for default stream (av_find_best_stream)
  }
}
