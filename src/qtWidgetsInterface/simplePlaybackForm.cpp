#include "simplePlaybackForm.h"
#include "ui_simplePlaybackForm.h"

#include <QPainter>
#include <QString>

simplePlaybackForm::simplePlaybackForm(QString& mMediaSource, QWidget *parent) :
  QWidget(parent),
  playerEngine(mMediaSource.toStdString(), AV_PIX_FMT_RGB24, AV_SAMPLE_FMT_FLT),
  mSimpleAudio(new simpleAudio(&playerEngine, this)),
  ui(new Ui::simplePlaybackForm) {
  ui->setupUi(this);
  ui->simpleDisplayWidget->setEngine(&playerEngine);
  connect(this, &simplePlaybackForm::play, ui->simpleDisplayWidget, &simpleDisplay::setPlayFlag);
  connect(this, &simplePlaybackForm::play, mSimpleAudio, &simpleAudio::setPlayFlag);
  initThread = std::thread{&Mk01::engine::init, &playerEngine };
  ui->horizontalSlider->setSliderPosition(50);
}

simplePlaybackForm::~simplePlaybackForm() {
  delete ui;
  playerEngine.end();
  initThread.join();
}

void simplePlaybackForm::paintEvent(QPaintEvent*) {
  QPainter painter;
  painter.begin(this);
  if (const int elapsed = mTime.elapsed()) {
     QString framesPerSecond;
     framesPerSecond.setNum(mFrames /(elapsed / 1000.0), 'f', 2);
     painter.setPen(Qt::black);
     ui->label->setText(framesPerSecond + " paint calls / s");
  }
  painter.end();
  if (!(mFrames % 100)) {
      mTime.start();
      mFrames = 0;
  }
  ++mFrames;
  update();
}

void simplePlaybackForm::on_pushButton_clicked() {
  emit play();
}

void simplePlaybackForm::on_horizontalSlider_valueChanged(int value)
{
  mSimpleAudio->setVolume(value);
}
