#include <QCoreApplication>
#include <QtTest>

#include "Mk03/engineContainer.h"
#include "bitrateForm.h"
#include "playbackForm.h"

class bitratetest : public QObject {
  Q_OBJECT

 public slots:
  void setString(const QString& aString);

 public:
  bitratetest();
  ~bitratetest();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void testControls();

 private:
  std::shared_ptr<Mk03::engineContainer<>> pePtr;
  bitrateForm* mDialog;
  std::string mediaSource;
  QString mediaSourceQs;
};

void bitratetest::setString(const QString& aString) {
  mediaSourceQs = aString;
}

bitratetest::bitratetest() {}

bitratetest::~bitratetest() {}

void bitratetest::initTestCase() {
  mediaSource = "./../../../kiyosVideoAnalysisTool/test/video.mp4";
  try {
    mediaSourceQs = mediaSource.c_str();
    mDialog = new bitrateForm(mediaSourceQs);
  } catch (...) {
    QFAIL("probably can't find video.mp4 (wrong path)");
  }
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
}

void bitratetest::cleanupTestCase() {
  delete mDialog;
}

void bitratetest::testControls() {
  mDialog->on_playButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::playing);
  mDialog->on_pauseButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::paused);
  mDialog->on_stopButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::stopped);
  mDialog->on_playButton_clicked();
  QCOMPARE(mDialog->mPlayerState, playerState::playing);
}

QTEST_MAIN(bitratetest)

#include "tst_bitratetest.moc"
