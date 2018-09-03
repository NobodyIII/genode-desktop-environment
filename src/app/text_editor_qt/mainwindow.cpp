#include <QMessageBox>
#include <QIconEngine>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

QList<MainWindow *> MainWindow::_windows;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setWindowIcon(style()->standardIcon(QStyle::SP_FileIcon));
    _windows << this;
    ui->setupUi(this);
    ui->actionClose->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionSave_As->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->action_New->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->action_Open->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->action_Quit->setIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton));

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(closeAll()));
    connect(ui->action_New, SIGNAL(triggered()), this, SLOT(newWindow()));

    connect(ui->textArea, SIGNAL(modificationChanged(bool)), this, SLOT(updateTitle()));
    updateTitle();
    show();
}

MainWindow::~MainWindow()
{
    _windows.removeAll(this);
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!isModified()) {
        event->accept();
        deleteLater();
        return; /* close window */
    }

    QMessageBox msg(QMessageBox::Warning, "Close File", "The file \"" + filename + "\" has been modified.\nDo you want to save your changes or discard them?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    int ret = msg.exec();
    switch (ret) {
    case QMessageBox::Discard:
        event->accept();
        deleteLater();
        return;
    case QMessageBox::Save:
        if (saveFile())
            return; /* otherwise cancel */
    case QMessageBox::Cancel:
        event->ignore();
        return;
    }
}

bool MainWindow::saveFile()
{
    //TODO: try saving with filename from opening
    if (filename.isEmpty())
        return saveFileAs();
    else
        return _save();
}

bool MainWindow::saveFileAs()
{
    //TODO: add default filename
    QString saveName = Util::saveFileAs();
    if (saveName.isEmpty())
        return false;
    else {
        filename = saveName;
        return _save();
    }
}

bool MainWindow::_save()
{
    QString fileData = ui->textArea->toPlainText();
    QFile file(filename);

    if (!file.open(QFile::WriteOnly))
        return false;

    //TODO: handle incomplete write
    file.write(fileData.toUtf8());
    setModified(false);
    updateTitle(); /* force update to handle case when an unmodified file is saved under a new name */
    return true;
}

void MainWindow::updateTitle()
{
    QString name = filename;

    if (name.isEmpty())
        name = "Untitled";
    if (isModified())
        name += " [modified]";

    setWindowTitle(name);

}

void MainWindow::openFile()
{
    QString name = Util::openFile();

    if (name.isEmpty())
        return;

    QFile file(name);

    if (!file.open(QFile::ReadOnly))
        return;

    QTextStream stream(&file);

    MainWindow *win;
    if (ui->textArea->document()->isEmpty())
        win = this; /* when empty, use the same window rather than creating a new one */
    else
        win = new MainWindow;

    QString text = stream.readAll();
    win->ui->textArea->setPlainText(text);
    win->setModified(false);
    win->filename = name;
    win->updateTitle();
}

bool MainWindow::isModified()
{
    return ui->textArea->document()->isModified();
}

void MainWindow::setModified(bool modified)
{
    ui->textArea->document()->setModified(modified);
}

void MainWindow::closeAll()
{
    _closeAll();
}

void MainWindow::_closeAll()
{
    for (MainWindow *win : _windows) {
        win->close();
    }
}

void MainWindow::newWindow()
{
    /* the window automatically adds itself to the list */
    new MainWindow;
}
