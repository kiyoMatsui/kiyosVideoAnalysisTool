#ifndef SIMPLEAUDIO_H
#define SIMPLEAUDIO_H

#include <QAudioOutput>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <thread>
#include "simpleAudioIO.h"
#include "ffmpegUPtr.h"
#include "Mk01/engine.h"

class simpleAudio : public QObject
{
  Q_OBJECT

public:
  explicit simpleAudio(Mk01::engine* ptr, QObject *parent = nullptr);
  void setVolume(int pos);

protected:

signals:

public slots:
  void setPlayFlag();

private:
  void printError(QAudio::State state);

private:
  QAudioFormat mFormat;
  QAudioDeviceInfo mDevice;
  simpleAudioIO *mIOOutput;
  QAudioOutput *mAudioOutput;
  bool playFlag;

friend class simpleAudioIO;

};

#endif // SIMPLEAUDIO_H
