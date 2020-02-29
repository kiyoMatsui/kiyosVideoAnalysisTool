#include "playbackForm.h"
#include "ui_playbackForm.h"
#include "customExceptions.h"
#include "selectStreamDialog.h"
#include <QPainter>

playbackForm::playbackForm(QString& aMediaSource, QWidget *parent)
  : QWidget(parent)
  , mMediaSource(aMediaSource.toStdString())
  , mTime()
  , mFrames(0)
  , mPlayerState(playerState::stopped)
  , sliderHeldDown(false)
  , sizeFlag(false)
  , playerEngine(new Mk03::engineContainer<>(mMediaSource, AV_PIX_FMT_RGB24, AV_SAMPLE_FMT_FLT))
  , mFormat()
  , mDevice{QAudioDeviceInfo::defaultOutputDevice()}
  , mIOOutput(nullptr)
  , mAudioOutput(nullptr)
  , ui(new Ui::playbackForm) {
  ui->setupUi(this);
  ui->syncedDisplayWidget->setEngine(playerEngine.get());
  playerEngine->startThreads();
  ui->progressSlider->setRange(0, playerEngine->getDuration());
  ui->progressSlider->setSliderPosition(0);
  if(playerEngine->getAudioStreamIndex() >= 0) {
      initAudio();
      mAudioOutput->setVolume(0.5);
      ui->volumeSlider->setSliderPosition(50);
    }
  connect(this, &playbackForm::play, ui->syncedDisplayWidget, &syncedDisplay::setPlayFlag);
  connect(ui->syncedDisplayWidget, &syncedDisplay::emitProgressPtsXBase, this, &playbackForm::setProgress);
}

playbackForm::~playbackForm() {
  delete ui;
}

void playbackForm::initAudio() {
  assert(playerEngine);
  mFormat.setSampleRate(playerEngine->getSampleRate());
  mFormat.setChannelCount(playerEngine->getChannelCount());
  mFormat.setSampleSize(playerEngine->getSampleBits());
  mFormat.setCodec("audio/pcm");
  mFormat.setByteOrder(QAudioFormat::LittleEndian);
  mFormat.setSampleType(QAudioFormat::Float);
  QAudioDeviceInfo info(mDevice);
  if (!info.isFormatSupported(mFormat)) {
      qWarning() << "Default format not supported - trying to use nearest";
      mFormat = info.nearestFormat(mFormat);
  }
  mIOOutput = new syncedAudioIO(playerEngine.get(), this);
  mAudioOutput = new QAudioOutput(mDevice, mFormat, this);
  connect(mIOOutput, &syncedAudioIO::emitPtsXBase, ui->syncedDisplayWidget, &syncedDisplay::slotAudioPtsXBase_ms);
  connect(mAudioOutput, &QAudioOutput::stateChanged, this, &playbackForm::audioStateChange);
}

void playbackForm::paintEvent(QPaintEvent*) {
  QPainter painter;
  painter.begin(this);
  if (const int elapsed = mTime.elapsed()) {
     QString framesPerSecond;
     framesPerSecond.setNum(mFrames /(elapsed / 1000.0), 'f', 2);
     painter.setPen(Qt::black);
     ui->paintLabel->setText(framesPerSecond + " paint calls / s");
  }
  if(playerEngine->getExceptionPtr()) {
      std::exception_ptr temp = playerEngine->getExceptionPtr();
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

void playbackForm::audioStateChange(QAudio::State state) {
  qWarning() << "QAudioOutput state changed (id): " << state;
  if(state == QAudio::StoppedState) ui->progressSlider->setSliderPosition(0);
}

void playbackForm::setProgress(int64_t pts) {
  if(!sliderHeldDown) {
      ui->progressSlider->setSliderPosition(pts*1000);
    }
}

void playbackForm::on_playButton_clicked() {
  switch(mPlayerState) {
    case playerState::playing:
      break;
    case playerState::paused:
      emit play(true);
      playerEngine->play();
      if(playerEngine->getAudioStreamIndex() >= 0) mAudioOutput->resume();
      mPlayerState = playerState::playing;
      break;
    default:
      emit play(true);
      playerEngine->play();
      if(playerEngine->getAudioStreamIndex() >= 0) mAudioOutput->start(mIOOutput);
      mPlayerState = playerState::playing;
  }
}

void playbackForm::on_pauseButton_clicked() {
  switch(mPlayerState) {
    case playerState::playing:
      emit play(false);
      playerEngine->pause();
      if(playerEngine->getAudioStreamIndex() >= 0) mAudioOutput->suspend();
      mPlayerState = playerState::paused;
    case playerState::paused:
      break;
    default:
      break;
  }
}

void playbackForm::on_stopButton_clicked() {
  setStreams(playerEngine->getAudioStreamIndex(), playerEngine->getVideoStreamIndex());
}

void playbackForm::on_setSizeButton_clicked() {
  if(sizeFlag) {
      ui->setSizeButton->setText("Set Size");
      ui->syncedDisplayWidget->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
      ui->syncedDisplayWidget->setMinimumSize(0,0);
      sizeFlag = false;
    } else {
      ui->setSizeButton->setText("Fit window");
      ui->syncedDisplayWidget->setFixedSize(ui->widthSpinBox->value(), ui->heightSpinBox->value());
      sizeFlag = true;
    }
}

void playbackForm::on_settingsButton_clicked() {
  try {
    selectStreamDialog* mDialog = new selectStreamDialog(playerEngine.get(),this);
    connect(mDialog, &selectStreamDialog::emitStreamArguments, this, &playbackForm::setStreams);
    mDialog->show();
  } catch(...) {
    QMessageBox::critical(this, ("Opening settings failed"),
                          ("<p>Could not open settings page, please retry."),
                          QMessageBox::Abort );
  }
}

void playbackForm::on_volumeSlider_valueChanged(int value) {
  if(playerEngine->getAudioStreamIndex() >= 0) {
    assert(value >= 0);
    mAudioOutput->setVolume((double)value/100);
    }
}

void playbackForm::on_progressSlider_sliderPressed() {
  sliderHeldDown = true;
  on_pauseButton_clicked();
}

void playbackForm::on_progressSlider_sliderReleased() {
  sliderHeldDown = false;
  setStreams(playerEngine->getAudioStreamIndex(),playerEngine->getVideoStreamIndex(),ui->progressSlider->value()/1000);
  on_playButton_clicked();
}

void playbackForm::setStreams(int audioID, int videoID, int64_t seek_ms) {
  on_pauseButton_clicked();
  // ugly but required for simple seeking
  if(seek_ms <= 0) ui->progressSlider->setSliderPosition(0);
  if ((seek_ms * 1000) > playerEngine->getDuration() - 3000000) seek_ms = (playerEngine->getDuration()/1000) - 3000;
  if (seek_ms < 0 ) seek_ms = 0;
  AVRational tempTimeBase = playerEngine->getVideoTimeBase();
  int64_t tempSeek_pts_dummy = (seek_ms*tempTimeBase.den ) / (tempTimeBase.num * 1000) ;
  int64_t seek_ms_dummy = (tempSeek_pts_dummy * tempTimeBase.num * 1000) / (tempTimeBase.den );
  int64_t pendingSeek = (seek_ms_dummy*tempTimeBase.den ) / (tempTimeBase.num * 1000) ;
  try {
    disconnect(ui->syncedDisplayWidget, nullptr, nullptr, nullptr);
    disconnect(this, nullptr, nullptr, nullptr);
    int oldAudioID = playerEngine->getAudioStreamIndex();
    if(oldAudioID >= 0) {
        disconnect(mIOOutput, nullptr, nullptr, nullptr);
        disconnect(mAudioOutput, nullptr, nullptr, nullptr);
        mAudioOutput->stop();
      }
    playerEngine.reset(new Mk03::engineContainer<>(mMediaSource, AV_PIX_FMT_RGB24, AV_SAMPLE_FMT_FLT,audioID,videoID,pendingSeek));
    ui->syncedDisplayWidget->setEngine(playerEngine.get());
    playerEngine->startThreads();
    if(oldAudioID >= 0) {
        delete mIOOutput;
        delete mAudioOutput;
      }
    if(playerEngine->getAudioStreamIndex() >= 0) {
        initAudio();
        mAudioOutput->setVolume((double)(ui->volumeSlider->value())/100.0);
      }
    mPlayerState = playerState::stopped;
    connect(this, &playbackForm::play, ui->syncedDisplayWidget, &syncedDisplay::setPlayFlag);
    connect(ui->syncedDisplayWidget, &syncedDisplay::emitProgressPtsXBase, this, &playbackForm::setProgress);
  } catch (...) {
    QMessageBox::critical(this, ("Exception thrown"),
                          ("<p>Media player could not be reset, " \
                           "retry to resume playback."),
                          QMessageBox::Abort );
    this->close();
  }
}
