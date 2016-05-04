#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog() :
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::update_progress(uint time_est, quint64 size_done, quint64 size_full, quint64 speed)
{
    int progress=(100*size_done)/size_full;
    ui->progressBar->setValue(progress);
    QString time_text;
    if (time_est>3600) {
        time_text+=QString::number(time_est/3600)+" hours ";
    }
    if (time_est%3600>60) {
        time_text+=QString::number((time_est%3600)/60)+" minutes ";
        time_est%=60;
    }
    if (time_est<3600) {
        time_text+=QString::number(time_est%60)+" seconds ";
    }
    ui->estTimeText->setText(time_text);
}

void ProgressDialog::finished()
{
    close();
    deleteLater();
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    emit rejected();
    event->accept();
}
