#include <QCoreApplication>
#include <QtTest>

#include "Mk03/engineContainer.h"
#include "playbackForm.h"

class playbacktest : public QObject {
  Q_OBJECT

 public:
  playbacktest();
  ~playbacktest();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void testControls();

 private:
  std::shared_ptr<Mk03::engineContainer<>> pePtr;
  playbackForm* mDialog;
  std::string mediaSource;
  QString mediaSourceQs;
};

playbacktest::playbacktest() {}

playbacktest::~playbacktest() {}

void playbacktest::initTestCase() {
  mediaSource = "./../../../kiyosVideoAnalysisTool/test/video.mp4";
  try {
    mediaSourceQs = mediaSource.c_str();
    mDialog = new playbackForm(mediaSourceQs);
  } catch (...) {
    QFAIL("probably can't find video.mp4 (wrong path)");
  }
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
}

void playbacktest::cleanupTestCase() {
  delete mDialog;
}

void playbacktest::testControls() {
  mDialog->on_playButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::playing);
  mDialog->on_pauseButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::paused);
  mDialog->on_stopButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
  mDialog->on_playButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::playing);
}

QTEST_MAIN(playbacktest)

#include "tst_playbacktest.moc"
