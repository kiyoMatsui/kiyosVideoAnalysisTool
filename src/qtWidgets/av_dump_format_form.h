#ifndef AV_DUMP_FORMAT_FORM_H
#define AV_DUMP_FORMAT_FORM_H

#include <QWidget>

namespace Ui {
class av_dump_format_form;
}

class av_dump_format_form final : public QWidget {
  Q_OBJECT

 public:
  explicit av_dump_format_form(QWidget *parent = 0);
  ~av_dump_format_form();
  void displayOutput(char *str);

 private:
  Ui::av_dump_format_form *ui;
};

#endif  // AV_DUMP_FORMAT_FORM_H
