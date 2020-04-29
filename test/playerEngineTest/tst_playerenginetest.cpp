#include <QCoreApplication>
#include <QtTest>
#include <string>

#include "Mk03/engineContainer.h"

class playerenginetest : public QObject {
  Q_OBJECT

 public:
  playerenginetest();
  ~playerenginetest();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void test_build_data();
  void test_build();
  void test_audio();
  void test_video();

 private:
  std::shared_ptr<Mk03::engineContainer<>> pePtr;
  std::string mediaSource;
};

playerenginetest::playerenginetest() {}

playerenginetest::~playerenginetest() {}

void playerenginetest::initTestCase() {
  mediaSource = "./../../../kiyosVideoAnalysisTool/test/video.mp4";
  try {
    pePtr = std::make_shared<Mk03::engineContainer<>>(mediaSource,
                                               AV_PIX_FMT_RGB24, AV_SAMPLE_FMT_FLT);
  } catch(...) {
    QFAIL("probably can't find video.mp4 (wrong path)");
  }

  pePtr->startThreads();
  QCOMPARE(pePtr->mediaSource, mediaSource);
  QVERIFY(pePtr->mPlayerDemuxerPtr);
  QVERIFY(pePtr->mPlayerAudioDecPtr);
  QVERIFY(pePtr->mPlayerVideoDecPtr);
  QVERIFY(!(pePtr->exceptionPtr));
  QVERIFY(!(pePtr->exceptionFlag));
  QVERIFY(pePtr->mPlayerVideoDecPtr);
  QCOMPARE(pePtr->jointData.videoStreamIndex, 0);
  QCOMPARE(pePtr->jointData.audioStreamIndex, 1);
}

void playerenginetest::cleanupTestCase() {
  pePtr->end();
}

void playerenginetest::test_build_data() {
  QTest::addColumn<int>("audioIndex");
  QTest::addColumn<int>("videoIndex");
  QTest::addColumn<int>("audioIndexRes");
  QTest::addColumn<int>("videoIndexRes");

  QTest::newRow("VideoAudioNoSeek") <<  1  <<  0  <<  1 << 0;
  QTest::newRow("justVideoNoSeek")  << -1  <<  0  << -1 << 0;
  QTest::newRow("default")          << -42 << -42 <<  1 << 0;
  QTest::newRow("defaultNoAudio")   << -1  << -42 << -1 << 0;
}

void playerenginetest::test_build() {
  QFETCH(int, audioIndex);
  QFETCH(int, videoIndex);
  QFETCH(int, audioIndexRes);
  QFETCH(int, videoIndexRes);

  std::shared_ptr<Mk03::engineContainer<>> tempPtr = std::make_shared<Mk03::engineContainer<>>(
    mediaSource, AV_PIX_FMT_RGB24, AV_SAMPLE_FMT_FLT, audioIndex, videoIndex);

  QCOMPARE(tempPtr->jointData.audioStreamIndex, audioIndexRes);
  QCOMPARE(tempPtr->jointData.videoStreamIndex, videoIndexRes);
}

void playerenginetest::test_audio() {
  Mk03::audioBuffer* buf = pePtr->getAudioBuffer();
  QVERIFY(buf->buffer.data());
  QCOMPARE(buf->nbSamples, 1024);
  QVERIFY(buf->ptsXtimeBase_ms < 1000);
  pePtr->returnAudioBuffer(buf);
}

void playerenginetest::test_video() {
  Mk03::videoBuffer* buf = pePtr->getVideoBuffer();
  QVERIFY(buf->buffer.data());
  QCOMPARE(buf->pktPerFrame, 0);
  QVERIFY(buf->ptsXtimeBase_ms < 1000 );
  pePtr->returnVideoBuffer(buf);
}

QTEST_MAIN(playerenginetest)

#include "tst_playerenginetest.moc"
