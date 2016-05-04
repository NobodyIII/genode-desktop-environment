#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QCloseEvent>
//#include "taskworker.h"

class TaskWorker;

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog();
    ~ProgressDialog();

public slots:
    void update_progress(uint time_est, quint64 size_done, quint64 size_full, quint64 speed); /* units: sec, bytes, bytes, bytes/sec (quint64 to allow extreme speeds) */
    void finished();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
