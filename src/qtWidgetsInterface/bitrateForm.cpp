#include "bitrateForm.h"
#include "ui_bitrateForm.h"

bitrateForm::bitrateForm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::bitrateForm)
{
  ui->setupUi(this);
  QLinearGradient gradient(0, 0, 0, 400);
  gradient.setColorAt(0, QColor(190, 190, 190));
  gradient.setColorAt(0.38, QColor(205, 205, 205));
  gradient.setColorAt(1, QColor(170, 170, 170));
  ui->customPlot->setBackground(QBrush(gradient));
  QCPBars *bitrateBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);

  ui->customPlot->xAxis->setLabel("Frame");
  ui->customPlot->yAxis->setLabel("kbps");
  ui->customPlot->xAxis->setRange(0, 40);
  ui->customPlot->yAxis->setRange(0, 20000);

  //legend
  ui->customPlot->legend->setVisible(true);
  ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
  ui->customPlot->legend->setBrush(QColor(255, 255, 255, 100));
  ui->customPlot->legend->setBorderPen(Qt::NoPen);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->customPlot->legend->setFont(legendFont);
  ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  QVector<double> ticks;
  ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
  QVector<double> bitrate;
  bitrate  << 2000 << 2000 << 2000 << 2000 << 12000 << 800 << 4000;
  bitrateBars->setData(ticks, bitrate);

  /*
  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->customPlot->xAxis->setTicker(timeTicker);
  ui->customPlot->axisRect()->setupFullAxesBox();
  ui->customPlot->yAxis->setRange(-1.2, 1.2);

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, &QTimer::timeout, this, &bitrateForm::realtimeDataSlot);
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
  */
}

bitrateForm::~bitrateForm()
{
  delete ui;
}

//bad
void bitrateForm::realtimeDataSlot()
{
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.002) // at most add point every 2 ms
  {
    // add data to lines:
    ui->customPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
    ui->customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
    // rescale value (vertical) axis to fit the current data:
    //ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
  ui->customPlot->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
      /*
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
          , 0);
    lastFpsKey = key;
    frameCount = 0; */
  }
}

void bitrateForm::paintEvent(QPaintEvent*) {

}
