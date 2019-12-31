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

    void on_actionAVDumpFormat_triggered();

    void on_mainTabWidget_tabCloseRequested(int index);

    void on_mainTabWidget_tabBarDoubleClicked(int index);

private:
    QString mMediaSource = "" ;
    Ui::mainWindow *ui;
};

inline bool catchAllExceptionDialog(mainWindow* thisPtr) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(thisPtr, thisPtr->tr("catch all exception"),
                          thisPtr->tr("<p>Catch all exception thrown! " \
                             "Something weird happened and we arn't sure what." \
                             "<p>The action is stopped, hopefully this action is exception safe " \
                             "and you can just try it again if you work out what's wrong."),
                          QMessageBox::Abort | QMessageBox::Retry );
    if (reply == QMessageBox::Abort)
        return false;
    else
        return true;
}

inline bool stdExceptionDialog(mainWindow* thisPtr, const std::exception& e) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(thisPtr, thisPtr->tr("std exception"),
                          thisPtr->tr("<p>Standard exception thrown! " \
                             "<p>The action is stopped, hopefully this action is exception safe " \
                             "and you can just try it again. " \
                             "Exception: ") + QString(e.what()),
                          QMessageBox::Abort | QMessageBox::Retry );
    if (reply == QMessageBox::Abort)
        return false;
    else
        return true;
}

inline void mediaSourceNotSetExceptionDialog(mainWindow* thisPtr) {
    QMessageBox::critical(thisPtr, thisPtr->tr("Media source not set"),
                          thisPtr->tr("<p>Exception thrown! Media source is likely not set, " \
                             "so please reset it by clicking on Open Media and input the url or file path." \
                             "<p>The action is stopped, hopefully this action is exception safe " \
                             "and you can try again after setting the media source."),
                          QMessageBox::Abort );
}

inline void mediaSourceWrongExceptionDialog(mainWindow* thisPtr) {
    QMessageBox::critical(thisPtr, thisPtr->tr("Media source not found"),
                          thisPtr->tr("<p>Exception thrown! Media source not found, " \
                             "so please reset it by clicking on Open Media and input the url or file path." \
                             "<p>The action is stopped, hopefully this action is exception safe " \
                             "and you can try again after setting the media source."),
                          QMessageBox::Abort );
}
#endif // MAINWINDOW_H
