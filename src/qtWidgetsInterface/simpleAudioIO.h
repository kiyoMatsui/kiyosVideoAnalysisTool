#ifndef SIMPLEAUDIOIO_H
#define SIMPLEAUDIOIO_H

#include <QAudioOutput>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <thread>
#include "ffmpegUPtr.h"
#include "Mk01/engine.h"

class simpleAudioIO : public QIODevice
{
  Q_OBJECT

public:
  explicit simpleAudioIO(Mk01::engine* ptr, QObject *parent = nullptr);

protected:
  qint64 readData(char *data, qint64 maxlen) override;
  qint64 writeData(const char *data, qint64 len) override;
  qint64 bytesAvailable() const override;

signals:

public slots:
  void setPlayFlag();

private:

  Mk01::engine* pePtr;
  int audioFrameSize;
  bool playFlag;

friend class simpleAudio;
};

#endif // SIMPLEAUDIOIO_H
