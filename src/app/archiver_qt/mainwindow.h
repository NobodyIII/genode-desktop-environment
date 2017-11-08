#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QAction>

#include "archiveitemmodel.h"

//TODO: connect all actions to slots

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ArchiveItemModel _arcModel;


public slots:
    void slotSave();
    void slotSaveAs();
    void slotOpen();
};

#endif // MAINWINDOW_H
