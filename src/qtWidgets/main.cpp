#include "mainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication a(argc, argv);
  mainWindow w;
  w.show();

  return a.exec();
}
