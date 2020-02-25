#include "appinfo.h"
#include "syncedAudioIO.h"
#include <QDebug>


syncedAudioIO::syncedAudioIO(Mk02::engineContainer* ptr, QObject *parent)
  : QIODevice(parent)
  , pePtr(ptr)
  , channelXSampleBytes(ptr->getChannelCount()*ptr->getSampleBytes())
  , playBuf(nullptr) {
  open(QIODevice::ReadOnly);
}

qint64 syncedAudioIO::readData(char *data, qint64 len) {
  assert( len == 0);
  if (len != 0) {
      int bufferSize = 0;
      bool isFirstFrame = true;
      int64_t emitPtsValue;
      while(1) {
          if(!playBuf) playBuf = pePtr->getAudioBuffer();
          if (!playBuf) {
              break;
            }
          if ((bufferSize/channelXSampleBytes + playBuf->nbSamples) > (int)len/channelXSampleBytes) {
              break;
            }
          if(isFirstFrame) {
              emitPtsValue = playBuf->ptsXtimeBase_ms;
              isFirstFrame = false;
            }
          memcpy(data+bufferSize, reinterpret_cast<char*>(playBuf->buffer.data()), channelXSampleBytes*playBuf->nbSamples);
          bufferSize += channelXSampleBytes*playBuf->nbSamples;
          pePtr->returnAudioBuffer(playBuf);
          playBuf = nullptr;
          if ((qint64)bufferSize >= len || (qint64)bufferSize >= 2048*channelXSampleBytes) {
              break;
            }
        }
      emitPtsXBase(emitPtsValue);
      return bufferSize;
    }
  return 0;
}

qint64 syncedAudioIO::writeData(const char *data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

qint64 syncedAudioIO::bytesAvailable() const {
  return 0;
}




