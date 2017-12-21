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

    /* re-used dialogs */
    QString _inputExtractPath();
    QString _inputSavePath();

public slots:
    void slotNew();
    void slotOpen();
    void slotClose();
    void slotExtractAll();
    void slotExtractSelection();
    void slotProperties();
    void slotSaveAs();
    void slotFileOpen();
    void slotDelete();
    void slotAddFiles();
};

#endif // MAINWINDOW_H
