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
  //mDialog->setMediaSource(testString); //or
  mDialog->ui->lineEdit->setFocus();
  QTest::keyClicks(QApplication::focusWidget(), testString);
  QVERIFY2(mDialog->ui->lineEdit->hasFocus(), "lineedit did not focus");
  mDialog->on_buttonBox_accepted();
  QCOMPARE(mDialog->ui->lineEdit->text(), QString(" test String   "));
  QCOMPARE(testString, QString("test String"));
  QApplication::setActiveWindow(mDialog);
  mDialog->ui->lineEdit->setText("Error should not be set"); //not validated so should not be "set"
  QTest::keyClick(mDialog, Qt::Key_Escape);
  QCOMPARE(testString, QString("test String"));
}

QTEST_MAIN(openmediatest)

#include "tst_openmediatest.moc"
