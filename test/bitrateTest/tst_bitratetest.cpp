#include <QCoreApplication>
#include <QtTest>

#include "Mk03/engineContainer.h"
#include "bitrateForm.h"
#include "playbackForm.h"
#include "ui_bitrateForm.h"

class bitratetest : public QObject {
  Q_OBJECT

 public:
  bitratetest();
  ~bitratetest();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void testControls();

 private:
  std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> pePtr;
  bitrateForm* mDialog;
  std::string mediaSource;
  QString mediaSourceQs;
};

bitratetest::bitratetest() {}

bitratetest::~bitratetest() {}

void bitratetest::initTestCase() {
  mediaSource = "./../../../kiyosVideoAnalysisTool/test/video.mp4";
  try {
    pePtr = std::make_shared<Mk03::engineContainer<Mk03::bitrateAnalysis>>(mediaSource, AV_PIX_FMT_YUV420P, AV_SAMPLE_FMT_FLT);
    mediaSourceQs = QString(mediaSource.c_str());
    mDialog = new bitrateForm(pePtr, mediaSourceQs);
    pePtr->end(); //stop player just test form
  } catch (...) {
    QFAIL("probably can't find video.mp4 (wrong path)");
  }
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
}

void bitratetest::cleanupTestCase() {
  delete mDialog;
}

void bitratetest::testControls() {
  QApplication::setActiveWindow(mDialog);
  QSignalSpy sigspy(mDialog, &bitrateForm::play);
  QTest::mouseClick(mDialog->ui->playButton, Qt::LeftButton);
  QCOMPARE(mDialog->mPlayerState, playerState::playing);
  QTest::mouseClick(mDialog->ui->pauseButton, Qt::LeftButton);
  QCOMPARE(mDialog->mPlayerState, playerState::paused);
  QTest::mouseClick(mDialog->ui->stopButton, Qt::LeftButton);
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
  QCOMPARE(sigspy.count(), 2);
}

QTEST_MAIN(bitratetest)

#include "tst_bitratetest.moc"
