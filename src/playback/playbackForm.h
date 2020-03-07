#ifndef PLAYBACKFORM_H
#define PLAYBACKFORM_H


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

namespace Ui {
  class playbackForm;
}
enum class playerState {playing, paused, stopped };
class playbackForm final : public QWidget
{
  Q_OBJECT

public:
  playbackForm(QString& mMediaSource, QWidget *parent = nullptr);
  playbackForm(std::shared_ptr<Mk03::engineContainer<>> aPlayerEngine, QString& mMediaSource, QWidget *parent = nullptr);
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
  void setStreams(int a, int v, int64_t seek_pts = -1);
  void initAudio();

  std::string mMediaSource;
  QTime mTime;
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

#endif // PLAYBACKFORM_H
