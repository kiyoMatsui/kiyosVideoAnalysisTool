#ifndef BITRATEFORM_H
#define BITRATEFORM_H

#include <QAudioOutput>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <thread>
#include "Mk03/engineContainer.h"
#include "playbackForm.h"
#include "qcustomplot.h"

namespace Ui {
class bitrateForm;
}

class bitrateForm : public QWidget {
  Q_OBJECT

  friend class bitratetest;

 public:
  bitrateForm(std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> aPlayerEngine,
                         QString& aMediaSource, QWidget* parent = nullptr);
  ~bitrateForm();

 signals:
  void play(bool flag);

 private slots:
  void on_playButton_clicked();

  void on_pauseButton_clicked();

  void on_stopButton_clicked();

  void on_settingsButton_clicked();

  void on_progressSlider_sliderPressed();

  void on_progressSlider_sliderReleased();

 private:
  void paintEvent(QPaintEvent* event) override;
  void audioStateChange(QAudio::State state);
  void setProgress(int64_t pts);
  void setStreams(int audioID, int videoID, int64_t seek_ms = -1);
  void initAudio();

  std::string mMediaSource;
  QTime mTime;
  int mFrames;
  playerState mPlayerState;
  bool sliderHeldDown;
  bool sizeFlag;
  QTimer dataTimer;
  std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> playerEngine;
  double fpsInterval;
  double sumOfFpsInterval;
  int sumOfpktSizes;
  int secondsElapsed;
  int frameCount;
  QCPBars* bitrateBars;
  Ui::bitrateForm* ui;
};

#endif  // BITRATEFORM_H
