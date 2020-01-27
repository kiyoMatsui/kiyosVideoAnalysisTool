#include "simpleAudio.h"
#include <iostream>



simpleAudio::simpleAudio(Mk01::engine* ptr, QObject *parent)
  : QObject(parent)
  , mDevice(QAudioDeviceInfo::defaultOutputDevice())
  , mIOOutput(nullptr)
  , mAudioOutput(nullptr)
  , playFlag(false) {
  mFormat.setSampleRate(ptr->getSampleRate());
  mFormat.setChannelCount(ptr->getChannelCount());
  mFormat.setSampleSize(ptr->getSampleBytes()*Mk01::bitsPerByte);
  mFormat.setCodec("audio/pcm");
  mFormat.setByteOrder(QAudioFormat::LittleEndian);
  mFormat.setSampleType(QAudioFormat::Float);
  QAudioDeviceInfo info(mDevice);
  if (!info.isFormatSupported(mFormat)) {
      //qWarning() << "Default format not supported - trying to use nearest";
      mFormat = info.nearestFormat(mFormat);
  }
  mIOOutput = new simpleAudioIO(ptr, this);
  mAudioOutput = new QAudioOutput(mDevice, mFormat, this);
  mAudioOutput->setVolume(0.5);
  mAudioOutput->setNotifyInterval(5);

  connect(mAudioOutput, &QAudioOutput::stateChanged, this, &simpleAudio::printError);
}

void simpleAudio::setPlayFlag() {
  playFlag = true;
  mIOOutput->setPlayFlag();
  mAudioOutput->start(mIOOutput);
}

void simpleAudio::printError(QAudio::State state) {
  std::cout << "QAudioOutput state changed: " << state << std::endl;
}

void simpleAudio::setVolume(int pos) {
  mAudioOutput->setVolume((double)pos/100);
}
