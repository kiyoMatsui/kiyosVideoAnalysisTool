#ifndef SYNCEDAUDIOIO_H
#define SYNCEDAUDIOIO_H

#include <QAudioOutput>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <thread>
#include "ffmpegUPtr.h"
#include "Mk02/engineContainer.h"


class syncedAudioIO : public QIODevice
{
  Q_OBJECT

public:
  explicit syncedAudioIO(Mk02::engineContainer* ptr, QObject *parent = nullptr);

protected:
  qint64 readData(char *data, qint64 maxlen) override;
  qint64 writeData(const char *data, qint64 len) override;
  qint64 bytesAvailable() const override;

signals:
  void emitPtsXBase(int64_t pts);

public slots:

private:
  Mk02::engineContainer* pePtr;
  int channelXSampleBytes;
  Mk02::audioBuffer* playBuf;

friend class displayForm;
};

#endif // SYNCEDAUDIOIO_H
