#include "appinfo.h"
#include "mainWindow.h"
#include "mainWindowDialogs.h"
#include "ui_mainWindow.h"
#include "customExceptions.h"
#include "openMediaDialog.h"
#include "av_dump_format_form.h"
#include "avDumpFormat.h"
#include "simplePlaybackForm.h"
#include "bitrateForm.h"
#include "displayForm.h"

#include <QInputDialog>
#include <QLineEdit>
#include <memory>


mainWindow::mainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::mainWindow) {
  ui->setupUi(this);
}

mainWindow::~mainWindow() {
  delete ui;
}

void mainWindow::on_actionOpenMedia_triggered() {
    try {
      openMediaDialog* mediaDialog = new openMediaDialog(this);
      connect(this, &mainWindow::sendMediaSource, mediaDialog, &openMediaDialog::setMediaSource);
      connect(mediaDialog, &openMediaDialog::returnMediaSource, this, &mainWindow::setMediaSource);
      emit sendMediaSource(mMediaSource);
      mediaDialog->show();
    } catch(const std::exception& e) {
        stdExceptionDialog(this, e) == true ? on_actionOpenMedia_triggered() : (void)0 ;
    } catch(...) {
        catchAllExceptionDialog(this) == true ? on_actionOpenMedia_triggered() : (void)0 ;
    }
}

void mainWindow::on_actionMetadata_triggered() {
    try {
/* throw std::logic_error("got it!"); */   /* throw mediaSourceNotSetException(); */   /* throw 42; */
        mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0 ;
        bool ok;
        int i = QInputDialog::getInt(this, tr("set av_dump_format index"),
                                     tr("index: "), 0, 0, 99, 1, &ok);
        if(!ok) return;
        auto obj = std::make_unique<qtWidgetsInterface::avDumpFormat>(mMediaSource.toStdString(), i);
        char* lBuffer = obj->getBuffer();
        av_dump_format_form* tab = new av_dump_format_form(ui->mainTabWidget);
        tab->setObjectName(QString("metadata ")+QString::number(i));
        ui->mainTabWidget->addTab(tab, QString("metadata ")+QString::number(i));
        tab->displayOutput(lBuffer);
        ui->mainTabWidget->setCurrentWidget(tab);
    } catch(const mediaSourceNotSetException&) {
        mediaSourceNotSetExceptionDialog(this);
    } catch(const mediaSourceWrongException&) {
        mediaSourceWrongExceptionDialog(this);
    } catch(const std::exception& e) {
        stdExceptionDialog(this, e) == true ? on_actionMetadata_triggered() : (void)0 ;
    } catch(...) {
        catchAllExceptionDialog(this) == true ? on_actionMetadata_triggered() : (void)0 ;
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
      QString text = QInputDialog::getText(this, tr("Change name"),
                                         tr("Change tab name"),
                                         QLineEdit::Normal,
                                         ui->mainTabWidget->tabText(index), &ok);
      ok == true ? ui->mainTabWidget->setTabText(index, text) : (void)false ;
    } catch(...) {
    catchAllExceptionNoRetryDialog(this);
    }
}

void mainWindow::setMediaSource(QString aString)
{
    mMediaSource = aString;
}





void mainWindow::on_actionSimplePlayback_triggered() {
  try {
    mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0 ;
    simplePlaybackForm* tab = new simplePlaybackForm(mMediaSource, ui->mainTabWidget);
    tab->setObjectName(QString("Playback"));
    ui->mainTabWidget->addTab(tab, QString("Playback"));
    ui->mainTabWidget->setCurrentWidget(tab);
  } catch(const mediaSourceNotSetException&) {
    mediaSourceNotSetExceptionDialog(this);
  } catch(const std::exception& e) {
    if (QString(e.what()) == QString("invalidUrl")) {
        mediaSourceWrongExceptionDialog(this);
      } else {
        stdExceptionDialog(this, e) == true ? on_actionSimplePlayback_triggered() : (void)0 ;
      }
  } catch(...) {
    catchAllExceptionNoRetryDialog(this);
  }

}

void mainWindow::on_actionAnalyseBitrate_triggered()
{
  try {
/* throw std::logic_error("got it!"); */   /* throw mediaSourceNotSetException(); */   /* throw 42; */
      mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0 ;
      bitrateForm* tab = new bitrateForm(ui->mainTabWidget);
      tab->setObjectName(QString("bitrate "));
      ui->mainTabWidget->addTab(tab, QString("bitrate "));
      ui->mainTabWidget->setCurrentWidget(tab);
  } catch(const mediaSourceNotSetException&) {
      mediaSourceNotSetExceptionDialog(this);
  } catch(const mediaSourceWrongException&) {
      mediaSourceWrongExceptionDialog(this);
  } catch(const std::exception& e) {
      stdExceptionDialog(this, e) == true ? on_actionAnalyseBitrate_triggered() : (void)0 ;
  } catch(...) {
      catchAllExceptionDialog(this) == true ? on_actionAnalyseBitrate_triggered() : (void)0 ;
  }
}

void mainWindow::on_actionPlayback_triggered() {
  try {
    mMediaSource == "" ? throw mediaSourceNotSetException() : (void)0 ;
    displayForm* tab = new displayForm(mMediaSource, ui->mainTabWidget);
    tab->setObjectName(QString("Playback"));
    ui->mainTabWidget->addTab(tab, QString("Playback"));
    ui->mainTabWidget->setCurrentWidget(tab);
  } catch(const mediaSourceNotSetException&) {
    mediaSourceNotSetExceptionDialog(this);
  } catch(const std::exception& e) {
    if (QString(e.what()) == QString("invalidUrl")) {
        mediaSourceWrongExceptionDialog(this);
      } else {
        stdExceptionDialog(this, e) == true ? on_actionPlayback_triggered() : (void)0 ;
      }
  } catch(...) {
    catchAllExceptionNoRetryDialog(this);
  }
}
