#include "appinfo.h"
#include "simplePlaybackForm.h"
#include "ui_simplePlaybackForm.h"
#include "mainWindowDialogs.h"
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
  playerEngine.init();
  ui->horizontalSlider->setSliderPosition(50);
}

simplePlaybackForm::~simplePlaybackForm() {
  delete ui;
  playerEngine.end();
}

void simplePlaybackForm::paintEvent(QPaintEvent*) {
  QPainter painter;
  painter.begin(this);
  if (const int elapsed = mTime.elapsed()) {
     QString framesPerSecond;
     framesPerSecond.setNum(mFrames /(elapsed / 1000.0), 'f', 2);
     painter.setPen(Qt::black);
     ui->paintLabel->setText(framesPerSecond + " paint calls / s");
  }
  if(playerEngine.getExceptionPtr()) {
      std::exception_ptr temp = playerEngine.getExceptionPtr();
      try {
        if(temp) {
            std::rethrow_exception(temp);
          }
      } catch(const std::exception& e) {
        ui->exceptionLabel->setText("Player engine exception: " + QString(e.what()));
      }
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

void simplePlaybackForm::on_horizontalSlider_valueChanged(int value) {
  assert(value >= 0);
  mSimpleAudio->setVolume(value);
}
