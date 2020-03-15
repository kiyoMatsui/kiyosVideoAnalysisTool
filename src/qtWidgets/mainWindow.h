#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <exception>
#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QTimer>

#include "Mk03/engineContainer.h"

namespace metadata {
  class avDumpFormat;
}
enum class sourceType{ local=0, DASH, HLS}; // use static_cast<int> to get index

template <typename T>
class asyncMembers {
public:
  explicit asyncMembers(unsigned int& aCounter)
    : counter(aCounter)
    , exceptionPtr(nullptr)
    , watcher()
    , clickedFlag(false) {
  }
  void reset() {
    counter--;
    clickedFlag.exchange(false);
    exceptionPtr = nullptr;
  }

  unsigned int& counter;
  std::exception_ptr exceptionPtr;
  QFutureWatcher<T> watcher;
  std::atomic<bool> clickedFlag;
};

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit mainWindow(QWidget *parent = 0);
  ~mainWindow();

private:
  void paintEvent(QPaintEvent *event) override;

public slots:
  void setMediaSource(QString aString);


signals:
  void sendMediaSource(QString aMediaSource);

private slots:
  void on_actionOpenMedia_triggered();

  void on_actionMetadata_triggered();

  void on_mainTabWidget_tabCloseRequested(int index);

  void on_mainTabWidget_tabBarDoubleClicked(int index);

  void on_actionToggle_tabs_closeable_triggered();

  void on_actionWhat_is_this_triggered();

  void on_actionSource_code_triggered();

  void on_actionAnalyseBitrate_triggered();

  void on_actionPlayback_triggered();


  void metadataCallback(); //if slot follows the signature style (slot name!) it will moan with user defined slots.

  void analyseBitrateCallback();

  void PlaybackCallback();

private:
  QString mMediaSource = "" ;
  bool closableTabFlag;
  unsigned int loadingCount;
  bool loadingDoneFlag;
  asyncMembers<std::shared_ptr<metadata::avDumpFormat>> on_actionMetadata_triggeredAM;
  asyncMembers<std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>>> on_actionAnalyseBitrate_triggeredAM;
  asyncMembers<std::shared_ptr<Mk03::engineContainer<>>> on_actionPlayback_triggeredAM;
  Ui::mainWindow *ui;
};


#endif // MAINWINDOW_H
