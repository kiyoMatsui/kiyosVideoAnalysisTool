#include "mainWindow.h"
#include "appinfo.h"
#include "avDumpFormat.h"
#include "av_dump_format_form.h"
#include "bitrateForm.h"
#include "customExceptions.h"
#include "mainWindowDialogs.h"
#include "openMediaDialog.h"
#include "playbackForm.h"
#include "ui_mainWindow.h"

#include <QFutureWatcher>
#include <QInputDialog>
#include <QLineEdit>
#include <QtConcurrent>
#include <memory>

mainWindow::mainWindow(QWidget* parent)
    : QMainWindow(parent),
      closableTabFlag(false),
      loadingCount(0),
      loadingDoneFlag(false),
      on_actionMetadata_triggeredAM(loadingCount),
      on_actionAnalyseBitrate_triggeredAM(loadingCount),
      on_actionPlayback_triggeredAM(loadingCount),
      ui(new Ui::mainWindow) {
  ui->setupUi(this);
  connect(&on_actionMetadata_triggeredAM.watcher, &QFutureWatcher<std::shared_ptr<metadata::avDumpFormat>>::finished,
          this, &mainWindow::metadataCallback);
  connect(&on_actionAnalyseBitrate_triggeredAM.watcher,
          &QFutureWatcher<std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>>>::finished, this,
          &mainWindow::analyseBitrateCallback);
  connect(&on_actionPlayback_triggeredAM.watcher, &QFutureWatcher<std::shared_ptr<Mk03::engineContainer<>>>::finished,
          this, &mainWindow::PlaybackCallback);
}

mainWindow::~mainWindow() {
  delete ui;
}

void mainWindow::paintEvent(QPaintEvent*) {
  if (loadingCount > 0) {
    loadingDoneFlag = true;
    if (ui->statusBar->currentMessage() == QString("   -")) {
      ui->statusBar->showMessage("   \\");
    } else if (ui->statusBar->currentMessage() == QString("   \\")) {
      ui->statusBar->showMessage("   |");
    } else if (ui->statusBar->currentMessage() == QString("   |")) {
      ui->statusBar->showMessage("   /");
    } else {
      ui->statusBar->showMessage("   -");
    }
  } else {
    if (loadingDoneFlag) {
      ui->statusBar->showMessage("Done", 10000);
      loadingDoneFlag = false;
    }
  }
}

void mainWindow::on_actionOpenMedia_triggered() {
  try {
    auto* mediaDialog = new openMediaDialog(this);
    connect(this, &mainWindow::sendMediaSource, mediaDialog, &openMediaDialog::setMediaSource);
    connect(mediaDialog, &openMediaDialog::returnMediaSource, this, &mainWindow::setMediaSource);
    emit sendMediaSource(mMediaSource);
    mediaDialog->show();
    disconnect(this, &mainWindow::sendMediaSource, mediaDialog, &openMediaDialog::setMediaSource);
  } catch (const std::exception& e) {
    stdExceptionDialog(this, e) == true ? on_actionOpenMedia_triggered() : (void)0;
  } catch (...) {
    catchAllExceptionDialog(this) == true ? on_actionOpenMedia_triggered() : (void)0;
  }
}

void mainWindow::on_actionMetadata_triggered() {
  if (!on_actionMetadata_triggeredAM.clickedFlag.load()) {
    // safe as long as slots are syncronised (they are!).
    on_actionMetadata_triggeredAM.clickedFlag.exchange(true);
    try {
      mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0;
      bool ok;
      int i = QInputDialog::getInt(this, tr("set av_dump_format index"), tr("index: "), 0, 0, 99, 1, &ok);
      if (!ok) return;
      QFuture future = QtConcurrent::run([=]() -> std::shared_ptr<metadata::avDumpFormat> {
        try {
          return std::make_shared<metadata::avDumpFormat>(mMediaSource.toStdString(), i);
        } catch (...) {
          on_actionMetadata_triggeredAM.exceptionPtr = std::current_exception();
          return nullptr;
        }
      });
      loadingCount++;
      on_actionMetadata_triggeredAM.watcher.setFuture(future);
    } catch (const mediaSourceNotSetException&) {
      mediaSourceNotSetExceptionDialog(this);
      on_actionMetadata_triggeredAM.reset();
    } catch (const std::exception& e) {
      stdExceptionDialog(this, e) == true ? on_actionMetadata_triggered() : (void)0;
      on_actionMetadata_triggeredAM.reset();
    } catch (...) {
      catchAllExceptionDialog(this) == true ? on_actionMetadata_triggered() : (void)0;
      on_actionMetadata_triggeredAM.reset();
    }
  }
}

void mainWindow::metadataCallback() {
  try {
    if (on_actionMetadata_triggeredAM.exceptionPtr) std::rethrow_exception(on_actionMetadata_triggeredAM.exceptionPtr);
    auto* tab = new av_dump_format_form(ui->mainTabWidget);
    tab->setObjectName(QString("metadata ") + QString::number(on_actionMetadata_triggeredAM.watcher.result()->index));
    ui->mainTabWidget->addTab(
      tab, QString("metadata ") + QString::number(on_actionMetadata_triggeredAM.watcher.result()->index));
    char* lBuffer = on_actionMetadata_triggeredAM.watcher.result()->getBuffer();
    tab->displayOutput(lBuffer);
    ui->mainTabWidget->setCurrentWidget(tab);
    on_actionMetadata_triggeredAM.reset();
  } catch (const mediaSourceWrongException&) {
    mediaSourceWrongExceptionDialog(this);
    on_actionMetadata_triggeredAM.reset();
  } catch (const std::exception& e) {
    stdExceptionDialog(this, e) == true ? on_actionMetadata_triggered() : (void)0;
    on_actionMetadata_triggeredAM.reset();
  } catch (...) {
    catchAllExceptionDialog(this) == true ? on_actionMetadata_triggered() : (void)0;
    on_actionMetadata_triggeredAM.reset();
  }
}

void mainWindow::on_mainTabWidget_tabCloseRequested(int index) {
  assert(index > 0);
  QWidget* temp = ui->mainTabWidget->widget(index);
  ui->mainTabWidget->removeTab(index);
  delete temp;
}

void mainWindow::on_mainTabWidget_tabBarDoubleClicked(int index) {
  assert(index > 0);
  try {
    bool ok;
    QString text = QInputDialog::getText(this, tr("Change name"), tr("Change tab name"), QLineEdit::Normal,
                                         ui->mainTabWidget->tabText(index), &ok);
    ok == true ? ui->mainTabWidget->setTabText(index, text) : (void)false;
  } catch (...) {
    catchAllExceptionNoRetryDialog(this);
  }
}

void mainWindow::on_actionToggle_tabs_closeable_triggered() {
  if (closableTabFlag) {
    ui->mainTabWidget->setTabsClosable(closableTabFlag);
    closableTabFlag = false;
  } else {
    ui->mainTabWidget->setTabsClosable(closableTabFlag);
    closableTabFlag = true;
  }
}

void mainWindow::on_actionWhat_is_this_triggered() {
  QWhatsThis::enterWhatsThisMode();
}

void mainWindow::on_actionSource_code_triggered() {
  QDesktopServices::openUrl(QUrl("https://github.com/kiyoMatsui/kiyosVideoAnalysisTool"));
}

void mainWindow::setMediaSource(QString aString) {
  mMediaSource = std::move(aString);
}

void mainWindow::on_actionAnalyseBitrate_triggered() {
  if (!on_actionAnalyseBitrate_triggeredAM.clickedFlag.load()) {
    on_actionAnalyseBitrate_triggeredAM.clickedFlag.exchange(true);
    try {
      mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0;
      QFuture future = QtConcurrent::run([=]() -> std::shared_ptr<Mk03::engineContainer<Mk03::bitrateAnalysis>> {
        try {
          return std::make_shared<Mk03::engineContainer<Mk03::bitrateAnalysis>>(mMediaSource.toStdString(),
                                                                                AV_PIX_FMT_YUV420P, AV_SAMPLE_FMT_FLT);
        } catch (...) {
          on_actionAnalyseBitrate_triggeredAM.exceptionPtr = std::current_exception();
          return nullptr;
        }
      });
      loadingCount++;
      on_actionAnalyseBitrate_triggeredAM.watcher.setFuture(future);
    } catch (const mediaSourceNotSetException&) {
      mediaSourceNotSetExceptionDialog(this);
      on_actionAnalyseBitrate_triggeredAM.reset();
    } catch (const std::exception& e) {
      if (QString(e.what()) == QString("invalidUrl")) {
        mediaSourceWrongExceptionDialog(this);
      } else {
        stdExceptionDialog(this, e) == true ? on_actionAnalyseBitrate_triggered() : (void)0;
      }
      on_actionAnalyseBitrate_triggeredAM.reset();
    } catch (...) {
      catchAllExceptionNoRetryDialog(this);
      on_actionAnalyseBitrate_triggeredAM.reset();
    }
  }
}

void mainWindow::analyseBitrateCallback() {
  try {
    if (on_actionAnalyseBitrate_triggeredAM.exceptionPtr)
      std::rethrow_exception(on_actionAnalyseBitrate_triggeredAM.exceptionPtr);
    bitrateForm* tab =
      new bitrateForm(on_actionAnalyseBitrate_triggeredAM.watcher.result(), mMediaSource, ui->mainTabWidget);
    tab->setObjectName(QString("bitrate "));
    ui->mainTabWidget->addTab(tab, QString("bitrate "));
    ui->mainTabWidget->setCurrentWidget(tab);
    on_actionAnalyseBitrate_triggeredAM.reset();
  } catch (const mediaSourceNotSetException&) {
    mediaSourceNotSetExceptionDialog(this);
    on_actionAnalyseBitrate_triggeredAM.reset();
  } catch (const std::exception& e) {
    if (QString(e.what()) == QString("invalidUrl")) {
      mediaSourceWrongExceptionDialog(this);
    } else {
      stdExceptionDialog(this, e) == true ? on_actionAnalyseBitrate_triggered() : (void)0;
    }
    on_actionAnalyseBitrate_triggeredAM.reset();
  } catch (...) {
    catchAllExceptionNoRetryDialog(this);
    on_actionAnalyseBitrate_triggeredAM.reset();
  }
}

void mainWindow::on_actionPlayback_triggered() {
  if (!on_actionPlayback_triggeredAM.clickedFlag.load()) {
    on_actionPlayback_triggeredAM.clickedFlag.exchange(true);
    try {
      mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0;
      QFuture future = QtConcurrent::run([=]() -> std::shared_ptr<Mk03::engineContainer<>> {
        try {
          return std::make_shared<Mk03::engineContainer<>>(mMediaSource.toStdString(), AV_PIX_FMT_RGB24,
                                                           AV_SAMPLE_FMT_FLT);
        } catch (...) {
          on_actionPlayback_triggeredAM.exceptionPtr = std::current_exception();
          return nullptr;
        }
      });
      loadingCount++;
      on_actionPlayback_triggeredAM.watcher.setFuture(future);
    } catch (const mediaSourceNotSetException&) {
      mediaSourceNotSetExceptionDialog(this);
      on_actionPlayback_triggeredAM.reset();
    } catch (const std::exception& e) {
      if (QString(e.what()) == QString("invalidUrl")) {
        mediaSourceWrongExceptionDialog(this);
      } else {
        stdExceptionDialog(this, e) == true ? on_actionPlayback_triggered() : (void)0;
      }
      on_actionPlayback_triggeredAM.reset();
    } catch (...) {
      catchAllExceptionNoRetryDialog(this);
      on_actionPlayback_triggeredAM.reset();
    }
  }
}

void mainWindow::PlaybackCallback() {
  try {
    if (on_actionPlayback_triggeredAM.exceptionPtr) std::rethrow_exception(on_actionPlayback_triggeredAM.exceptionPtr);
    playbackForm* tab =
      new playbackForm(on_actionPlayback_triggeredAM.watcher.result(), mMediaSource, ui->mainTabWidget);
    // playbackForm* tab = new playbackForm(mMediaSource, ui->mainTabWidget);
    tab->setObjectName(QString("Playback"));
    ui->mainTabWidget->addTab(tab, QString("Playback"));
    ui->mainTabWidget->setCurrentWidget(tab);
    on_actionPlayback_triggeredAM.reset();
  } catch (const mediaSourceNotSetException&) {
    mediaSourceNotSetExceptionDialog(this);
    on_actionPlayback_triggeredAM.reset();
  } catch (const std::exception& e) {
    if (QString(e.what()) == QString("invalidUrl")) {
      mediaSourceWrongExceptionDialog(this);
    } else {
      stdExceptionDialog(this, e) == true ? on_actionPlayback_triggered() : (void)0;
    }
    on_actionPlayback_triggeredAM.reset();
  } catch (...) {
    catchAllExceptionNoRetryDialog(this);
    on_actionPlayback_triggeredAM.reset();
  }
}
