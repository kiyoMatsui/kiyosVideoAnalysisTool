#ifndef BITRATEFORM_H
#define BITRATEFORM_H


#include "Mk03/engineContainer.h"
#include "syncedAudioIO.h"
#include <QWidget>
#include <QAudioOutput>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QTime>
#include <QString>
#include <thread>
#include <QTimer>
#include "playbackForm.h"
#include "qcustomplot.h"

namespace Ui {
  class bitrateForm;
}

class bitrateForm : public QWidget
{
  Q_OBJECT

public:
  bitrateForm(QString& mMediaSource, QWidget *parent = nullptr);
  bitrateForm(std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> aPlayerEngine, QString& mMediaSource, QWidget *parent = nullptr);
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
  void paintEvent(QPaintEvent *event) override;
  void audioStateChange(QAudio::State state);
  void setProgress(int64_t pts);
  void setStreams(int a, int v, int64_t seek_pts = -1);
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
  QCPBars *bitrateBars;
  Ui::bitrateForm *ui;
};

#endif // BITRATEFORM_H
