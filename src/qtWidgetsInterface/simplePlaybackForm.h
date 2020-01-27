#ifndef SIMPLEPLAYBACKFORM_H
#define SIMPLEPLAYBACKFORM_H

#include <QWidget>
#include <QTime>
#include <QString>
#include "simpleAudio.h"
#include "simpleDisplay.h"
#include "Mk01/engine.h"

namespace Ui {
  class simplePlaybackForm;
}

class simplePlaybackForm : public QWidget
{
  Q_OBJECT

public:
  explicit simplePlaybackForm(QString& str, QWidget *parent = 0);
  ~simplePlaybackForm();

signals:
  void play();

private slots:
  void on_pushButton_clicked();

  void on_horizontalSlider_valueChanged(int value);

private:
  void paintEvent(QPaintEvent *event) override;

  QTime mTime;
  int mFrames;
  std::thread initThread;
  Mk01::engine playerEngine;
  simpleAudio* mSimpleAudio;
  Ui::simplePlaybackForm *ui;

  friend class simpleDisplay;
  friend class simpleAudio;
};

#endif // SIMPLEPLAYBACKFORM_H
