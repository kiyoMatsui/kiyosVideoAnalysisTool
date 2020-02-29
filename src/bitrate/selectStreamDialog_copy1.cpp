#include "selectStreamDialog_copy1.h"
#include "ui_selectStreamDialog_copy1.h"


selectStreamDialog_copy1::selectStreamDialog_copy1(Mk03::engineContainer<Mk03::bitrateAnalysis>* aPePtr, QWidget *parent) :
  QDialog(parent),
  pePtr(aPePtr),
  ui(new Ui::selectStreamDialog_copy1) {
  ui->setupUi(this);
  ui->audioSpinBox->setRange(-1,pePtr->getStreams()-1);
  ui->videoSpinBox->setRange(0,pePtr->getStreams()-1);
  ui->audioSpinBox->setValue(pePtr->getAudioStreamIndex());
  ui->videoSpinBox->setValue(pePtr->getVideoStreamIndex());
}

selectStreamDialog_copy1::~selectStreamDialog_copy1()
{
  delete ui;
}

void selectStreamDialog_copy1::on_buttonBox_accepted()
{
  if ((pePtr->checkStreamType(ui->audioSpinBox->value()) == 1 || //AVMEDIA_TYPE_AUDIO = 1
      pePtr->checkStreamType(ui->audioSpinBox->value()) == -1 ) && // no audio  = -1
      pePtr->checkStreamType(ui->videoSpinBox->value()) == 0 ) { // AVMEDIA_TYPE_VIDEO = 0
      emit emitStreamArguments(ui->audioSpinBox->value(),ui->videoSpinBox->value());
    } else {
      QMessageBox::critical(this, ("stream index error"),
                           ("<p>Stream index wrong type! "
                            "<p>Hit Metadata to check which stream (index) is a video/audio type. " \
                            "<p>Streams set to default."),
                            QMessageBox::Abort );
      emit emitStreamArguments(-42,-42); // magic number for default stream (av_find_best_stream)
    }
}
