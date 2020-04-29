#include "bitrateForm.h"
#include <QPainter>
#include "customExceptions.h"
#include "selectStreamDialog_copy1.h"
#include "ui_bitrateForm.h"

bitrateForm::bitrateForm(QString& aMediaSource, QWidget* parent)
    : QWidget(parent),
      mMediaSource(aMediaSource.toStdString()),
      mFrames(0),
      mPlayerState(playerState::stopped),
      sliderHeldDown(false),
      sizeFlag(false),
      playerEngine(
        new Mk03::engineContainer<Mk03::bitrateAnalysis>(mMediaSource, AV_PIX_FMT_YUV420P, AV_SAMPLE_FMT_FLT)),
      fpsInterval(1.0 / playerEngine->getFps()),
      sumOfFpsInterval(0.0),
      sumOfpktSizes(0),
      secondsElapsed(0),
      frameCount(0),
      bitrateBars(nullptr),
      ui(new Ui::bitrateForm) {
  ui->setupUi(this);
  playerEngine->startThreads();
  ui->progressSlider->setRange(0, playerEngine->getDuration());
  ui->progressSlider->setSliderPosition(0);

  QLinearGradient gradient(0, 0, 0, 400);
  gradient.setColorAt(0, QColor(190, 190, 190));
  gradient.setColorAt(0.38, QColor(205, 205, 205));
  gradient.setColorAt(1, QColor(170, 170, 170));
  ui->customPlot->setBackground(QBrush(gradient));
  bitrateBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);

  ui->customPlot->xAxis->setLabel("Seconds");
  ui->customPlot->yAxis->setLabel("Bitrate (kbps)");
  ui->customPlot->xAxis->setRange(0, 60);
  ui->customPlot->yAxis->setRange(0, 2 * (playerEngine->getVideoBitrate() / 1000));
}

bitrateForm::bitrateForm(std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> aPlayerEngine,
                         QString& aMediaSource, QWidget* parent)
    : QWidget(parent),
      mMediaSource(aMediaSource.toStdString()),
      mFrames(0),
      mPlayerState(playerState::stopped),
      sliderHeldDown(false),
      sizeFlag(false),
      playerEngine(std::move(aPlayerEngine)),
      fpsInterval(1.0 / playerEngine->getFps()),
      sumOfFpsInterval(0.0),
      sumOfpktSizes(0),
      secondsElapsed(0),
      frameCount(0),
      bitrateBars(nullptr),
      ui(new Ui::bitrateForm) {
  ui->setupUi(this);
  playerEngine->startThreads();
  ui->progressSlider->setRange(0, playerEngine->getDuration());
  ui->progressSlider->setSliderPosition(0);

  QLinearGradient gradient(0, 0, 0, 400);
  gradient.setColorAt(0, QColor(190, 190, 190));
  gradient.setColorAt(0.38, QColor(205, 205, 205));
  gradient.setColorAt(1, QColor(170, 170, 170));
  ui->customPlot->setBackground(QBrush(gradient));
  bitrateBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);

  ui->customPlot->xAxis->setLabel("Seconds");
  ui->customPlot->yAxis->setLabel("Bitrate (kbps)");
  ui->customPlot->xAxis->setRange(0, 60);
  ui->customPlot->yAxis->setRange(0, 2 * (playerEngine->getVideoBitrate() / 1000));
}

bitrateForm::~bitrateForm() {
  delete ui;
}

void bitrateForm::paintEvent(QPaintEvent*) {
  QPainter painter;
  painter.begin(this);
  if (playerEngine->getExceptionPtr()) {
    std::exception_ptr temp = playerEngine->getExceptionPtr();
    try {
      if (temp) {
        std::rethrow_exception(temp);
      }
    } catch (const std::exception& e) {
      ui->exceptionLabel->setText("Player engine exception: " + QString(e.what()));
    }
  }
  painter.end();
  if (mPlayerState == playerState::playing) {
    bool firstFlag = true;
    for (int a = 0; a < 32; a++) {
      Mk03::audioBuffer* tempA = playerEngine->getAudioBuffer();
      Mk03::videoBuffer* tempV = playerEngine->getVideoBuffer();
      if (tempV && firstFlag) {
        setProgress(tempV->ptsXtimeBase_ms);
        firstFlag = false;
      }
      if (tempA) playerEngine->returnAudioBuffer(tempA);
      tempA = nullptr;
      if (tempV) {
        sumOfpktSizes += tempV->pktPerFrame;
        sumOfFpsInterval += fpsInterval;
        ++frameCount;
        if (sumOfFpsInterval > 1.0) {
          ++secondsElapsed;
          double Bitrate = (8 * sumOfpktSizes) / 1000;
          if (Bitrate > ui->customPlot->yAxis->range().size()) {
            ui->customPlot->yAxis->setRange(0, 2 * (Bitrate));
          }
          if (secondsElapsed > ui->customPlot->xAxis->range().size()) {
            ui->customPlot->xAxis->setRange(0, 2 * (ui->customPlot->xAxis->range().size()));
          }
          bitrateBars->addData(secondsElapsed, (8 * sumOfpktSizes) / 1000);
          ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
          sumOfFpsInterval -= 1.0;
          sumOfpktSizes = 0;
          frameCount = 0;
        }
        playerEngine->returnVideoBuffer(tempV);
        tempV = nullptr;
      }
      if (!tempA && !tempV) break;
    }
  }
  update();
}

void bitrateForm::audioStateChange(QAudio::State state) {
  qWarning() << "QAudioOutput state changed (id): " << state;
  if (state == QAudio::StoppedState) ui->progressSlider->setSliderPosition(0);
}

void bitrateForm::setProgress(int64_t pts) {
  if (!sliderHeldDown) {
    ui->progressSlider->setSliderPosition(pts * 1000);
  }
}

void bitrateForm::on_playButton_clicked() {
  switch (mPlayerState) {
    case playerState::playing:
      break;
    case playerState::paused:
      emit play(true);
      playerEngine->play();
      mPlayerState = playerState::playing;
      break;
    default:
      emit play(true);
      playerEngine->play();
      mPlayerState = playerState::playing;
  }
}

void bitrateForm::on_pauseButton_clicked() {
  switch (mPlayerState) {
    case playerState::playing:
      emit play(false);
      playerEngine->pause();
      mPlayerState = playerState::paused;
    case playerState::paused:
      break;
    default:
      break;
  }
}

void bitrateForm::on_stopButton_clicked() {
  setStreams(playerEngine->getAudioStreamIndex(), playerEngine->getVideoStreamIndex());
}

void bitrateForm::on_settingsButton_clicked() {
  try {
    auto* mDialog = new selectStreamDialog_copy1(playerEngine.get(), this);
    connect(mDialog, &selectStreamDialog_copy1::emitStreamArguments, this, &bitrateForm::setStreams);
    mDialog->show();
  } catch (...) {
    QMessageBox::critical(this, ("Opening settings failed"), ("<p>Could not open settings page, please retry."),
                          QMessageBox::Abort);
  }
}

void bitrateForm::on_progressSlider_sliderPressed() {
  sliderHeldDown = true;
  on_pauseButton_clicked();
}

void bitrateForm::on_progressSlider_sliderReleased() {
  sliderHeldDown = false;
  setStreams(playerEngine->getAudioStreamIndex(), playerEngine->getVideoStreamIndex(),
             ui->progressSlider->value() / 1000);
  on_playButton_clicked();
}

void bitrateForm::setStreams(int audioID, int videoID, int64_t seek_ms) {
  on_pauseButton_clicked();
  ui->customPlot->clearItems();
  ui->customPlot->clearPlottables();
  ui->customPlot->clearItems();
  bitrateBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
  secondsElapsed = 0;
  sumOfFpsInterval = 0.0;
  sumOfpktSizes = 0;
  frameCount = 0;
  // ugly but required for simple seeking
  if (seek_ms <= 0) ui->progressSlider->setSliderPosition(0);
  if ((seek_ms * 1000) > playerEngine->getDuration() - 3000000) seek_ms = (playerEngine->getDuration() / 1000) - 3000;
  if (seek_ms < 0) seek_ms = 0;
  AVRational tempTimeBase = playerEngine->getVideoTimeBase();
  int64_t tempSeek_pts_dummy = (seek_ms * tempTimeBase.den) / (tempTimeBase.num * 1000);
  int64_t seek_ms_dummy = (tempSeek_pts_dummy * tempTimeBase.num * 1000) / (tempTimeBase.den);
  int64_t pendingSeek = (seek_ms_dummy * tempTimeBase.den) / (tempTimeBase.num * 1000);
  try {
    disconnect(this, nullptr, nullptr, nullptr);
    playerEngine.reset(new Mk03::engineContainer<Mk03::bitrateAnalysis>(
      mMediaSource, AV_PIX_FMT_YUV420P, AV_SAMPLE_FMT_FLT, audioID, videoID, pendingSeek));
    playerEngine->startThreads();
    mPlayerState = playerState::stopped;
  } catch (...) {
    QMessageBox::critical(this, ("Exception thrown"),
                          ("<p>Media player could not be reset, "
                           "retry to resume playback."),
                          QMessageBox::Abort);
    this->close();
  }
}
