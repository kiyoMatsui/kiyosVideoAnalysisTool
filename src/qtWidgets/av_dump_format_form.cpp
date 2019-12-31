#include "av_dump_format_form.h"
#include "ui_av_dump_format_form.h"
#include "avDumpFormat.h"


av_dump_format_form::av_dump_format_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::av_dump_format_form)
{
    ui->setupUi(this);
}

av_dump_format_form::~av_dump_format_form()
{
    delete ui;
}

void av_dump_format_form::displayOutput(char* str)
{
    ui->mTextEdit->setPlainText(QString(str));
}
