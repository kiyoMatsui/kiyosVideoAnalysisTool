#ifndef PLAYBACKFORM_H
#define PLAYBACKFORM_H

#include <QAudioOutput>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QString>
#include <QElapsedTimer>
#include <QWidget>
#include <thread>
#include "Mk03/engineContainer.h"
#include "syncedAudioIO.h"

namespace Ui {
class playbackForm;
}
enum class playerState { playing, paused, stopped };
class playbackForm final : public QWidget {
  Q_OBJECT

  friend class playbacktest;

 public:
  playbackForm(std::shared_ptr<Mk03::engineContainer<>> aPlayerEngine, QString &aMediaSource,
               QWidget *parent = nullptr);
  ~playbackForm();

 signals:
  void play(bool flag);

 private slots:
  void on_playButton_clicked();

  void on_pauseButton_clicked();

  void on_setSizeButton_clicked();

  void on_stopButton_clicked();

  void on_volumeSlider_valueChanged(int value);

  void on_settingsButton_clicked();

  void on_progressSlider_sliderPressed();

  void on_progressSlider_sliderReleased();

 private:
  void paintEvent(QPaintEvent *event) override;
  void audioStateChange(QAudio::State state);
  void setProgress(int64_t pts);
  void setStreams(int audioID, int videoID, int64_t seek_ms = -1);
  void initAudio();

  std::string mMediaSource;
  QElapsedTimer mTime;
  int mFrames;
  playerState mPlayerState;
  bool sliderHeldDown;
  bool sizeFlag;
  std::shared_ptr<Mk03::engineContainer<>> playerEngine;
  QAudioFormat mFormat;
  QAudioDeviceInfo mDevice;
  syncedAudioIO *mIOOutput;
  QAudioOutput *mAudioOutput;
  Ui::playbackForm *ui;
};

#endif  // PLAYBACKFORM_H
