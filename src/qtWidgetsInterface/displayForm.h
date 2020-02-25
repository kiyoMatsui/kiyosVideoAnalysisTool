#ifndef DISPLAYFORM_H
#define DISPLAYFORM_H


#include "Mk02/engineContainer.h"
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
  class displayForm;
}
enum class playerState {playing, paused, stopped };
class displayForm : public QWidget
{
  Q_OBJECT

public:
  explicit displayForm(QString& mMediaSource, QWidget *parent = nullptr);
  ~displayForm();

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
  std::unique_ptr<Mk02::engineContainer> playerEngine;
  QAudioFormat mFormat;
  QAudioDeviceInfo mDevice;
  syncedAudioIO *mIOOutput;
  QAudioOutput *mAudioOutput;
  Ui::displayForm *ui;
};

#endif // DISPLAYFORM_H
