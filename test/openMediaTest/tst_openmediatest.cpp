#include <QCoreApplication>
#include <QtTest>

#include "openMediaDialog.h"
#include "ui_openMediaDialog.h"

class openmediatest : public QObject {
  Q_OBJECT

 public slots:
  void setString(const QString& aString);

 public:
  openmediatest();
  ~openmediatest();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void test_case1();

 private:
  openMediaDialog* mDialog;
  QString testString;
};

void openmediatest::setString(const QString& aString) { testString = aString; }

openmediatest::openmediatest() {}

openmediatest::~openmediatest() {}

void openmediatest::initTestCase() {
  mDialog = new openMediaDialog();
  testString = " test String   ";
  connect(mDialog, &openMediaDialog::returnMediaSource, this, &openmediatest::setString);
}

void openmediatest::cleanupTestCase() {
  delete mDialog;
}

void openmediatest::test_case1() {
  QApplication::setActiveWindow(mDialog);
  mDialog->setMediaSource(testString);
  mDialog->on_buttonBox_accepted();
  QCOMPARE(mDialog->ui->lineEdit->text(), QString(" test String   "));
  QCOMPARE(testString, QString("test String"));
}

QTEST_MAIN(openmediatest)

#include "tst_openmediatest.moc"
