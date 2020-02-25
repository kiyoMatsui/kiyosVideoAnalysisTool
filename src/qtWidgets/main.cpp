#include "appinfo.h"
#include "mainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication a(argc, argv);
  mainWindow w;
  w.show();
  //assert("app using runtime asserts" == "change src/appinfo to remove asserts");
  return a.exec();
}
