#include "simpleAudioIO.h"
#include <iostream>

simpleAudioIO::simpleAudioIO(Mk01::engine* ptr, QObject *parent)
  : QIODevice(parent)
  , pePtr(ptr)
  , audioFrameSize(ptr->getAudioFrameSize())
  , playFlag(false) {
}


qint64 simpleAudioIO::readData(char *data, qint64 len) {
  if (playFlag) {
      int bufferSize = 0;
      int n = 0;
      while(1) {
          Mk01::Buffer* mydata = pePtr->getAudioBuffer();
          if (!mydata) {
              break;
            }
          memcpy(data+bufferSize, reinterpret_cast<char*>(mydata->data()), audioFrameSize);
          pePtr->returnAudioBuffer(mydata);
          bufferSize += audioFrameSize;
          n++;
          if ((qint64)bufferSize > len || n == 2) {
              break;
            }
        }
      return bufferSize;
    }
  return 0;
}

qint64 simpleAudioIO::writeData(const char *data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 simpleAudioIO::bytesAvailable() const {
  return audioFrameSize;
}

void simpleAudioIO::setPlayFlag() {
  open(QIODevice::ReadOnly);
  playFlag = true;
}


