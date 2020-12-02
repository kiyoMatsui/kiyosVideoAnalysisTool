#include <QCoreApplication>
#include <QtTest>

#include "Mk03/engineContainer.h"
#include "playbackForm.h"
#include "ui_playbackForm.h"
//This test is broken, the form is too highly coupled with the player engine and unit testing framework is brearking something.
//Remember kids try to decouple classes...
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
    pePtr = std::make_shared<Mk03::engineContainer<>>(mediaSource, AV_PIX_FMT_YUV420P, AV_SAMPLE_FMT_FLT);
    mediaSourceQs = QString(mediaSource.c_str());
    mDialog = new playbackForm(pePtr, mediaSourceQs);
    pePtr->end(); //stop player just test form
  } catch (...) {
    QFAIL("probably can't find video.mp4 (wrong path)");
  }
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
}

void playbacktest::cleanupTestCase() {
  delete mDialog;
}

void playbacktest::testControls() {
  //QApplication::setActiveWindow(mDialog);
  //QSignalSpy sigspy(mDialog, &playbackForm::play);
  //QTest::mouseClick(mDialog->ui->playButton, Qt::LeftButton);
  //QCOMPARE(mDialog->mPlayerState, playerState::playing);
  //QTest::mouseClick(mDialog->ui->pauseButton, Qt::LeftButton);
  //QCOMPARE(mDialog->mPlayerState, playerState::paused);
  //QTest::mouseClick(mDialog->ui->stopButton, Qt::LeftButton);
  //QCOMPARE(mDialog->mPlayerState, playerState::stopped);
  //QCOMPARE(sigspy.count(), 2);
}

QTEST_MAIN(playbacktest)

#include "tst_playbacktest.moc"
