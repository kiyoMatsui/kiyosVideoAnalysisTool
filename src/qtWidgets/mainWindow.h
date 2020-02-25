#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>

enum class sourceType{ local=0, DASH, HLS}; // use static_cast<int> to get index

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit mainWindow(QWidget *parent = 0);
  ~mainWindow();

public slots:
  void setMediaSource(QString aString);

signals:
  void sendMediaSource(QString aMediaSource);

private slots:
  void on_actionOpenMedia_triggered();

  void on_actionMetadata_triggered();

  void on_mainTabWidget_tabCloseRequested(int index);

  void on_mainTabWidget_tabBarDoubleClicked(int index);

  void on_actionSimplePlayback_triggered();

  void on_actionAnalyseBitrate_triggered();

  void on_actionPlayback_triggered();

private:
  QString mMediaSource = "" ;
  Ui::mainWindow *ui;
};


#endif // MAINWINDOW_H
