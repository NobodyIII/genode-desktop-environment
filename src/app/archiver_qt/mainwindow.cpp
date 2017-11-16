#include <QPainter>
#include <qstylefactory.h>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << QStyleFactory::keys();
    QApplication::setDesktopSettingsAware(false);
    QApplication::setStyle("fusion");
//    setIconSize(QSize(32, 32));
    ui->setupUi(this);
    ui->archiveView->setModel(&_arcModel);
    ui->archiveView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    /* generate extract icon */
    //TODO: replace with more sensible solution
    qDebug() << style()->standardIcon(QStyle::SP_DirIcon).availableSizes();
    QPixmap *pm = new QPixmap(style()->standardIcon(QStyle::SP_DirIcon).pixmap(QSize(32, 32), QIcon::Normal, QIcon::On));
    QPainter painter(pm);
    QPoint pos((iconSize().width() - 16) / 2, 0);
    painter.drawPixmap(pos, style()->standardIcon(QStyle::SP_ArrowUp).pixmap(QSize(16, 16)));

    /* set icons */
    ui->actionAddFiles->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    ui->actionClose->setIcon(style()->standardIcon((QStyle::SP_DialogCloseButton)));
    ui->actionDelete->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->actionExtractSelection->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->actionExtractAll->setIcon(*pm);
    ui->actionFileOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionNew->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionProperties->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    ui->actionSaveAs->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    /* setup signal handling */
    connect(ui->actionAddFiles, SIGNAL(triggered(bool)), this, SLOT(slotAddFiles()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(slotClose()));
    connect(ui->actionDelete, SIGNAL(triggered(bool)), this, SLOT(slotDelete()));
    connect(ui->actionExtractAll, SIGNAL(triggered(bool)), this, SLOT(slotExtractAll()));
    connect(ui->actionExtractSelection, SIGNAL(triggered(bool)), this, SLOT(slotExtractSelection()));
    connect(ui->actionFileOpen, SIGNAL(triggered(bool)), this, SLOT(slotFileOpen()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(slotNew()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(slotOpen()));
    connect(ui->actionProperties, SIGNAL(triggered(bool)), this, SLOT(slotProperties()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(slotSaveAs()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotNew()
{
    //TODO: fill stub
}

void MainWindow::slotOpen()
{
    QString file_path = QFileDialog::getOpenFileName();
    if (!file_path.isEmpty())
        _arcModel.openArchive(file_path);
}

void MainWindow::slotClose()
{
    _arcModel.closeArchive();
}

void MainWindow::slotExtractAll()
{
    QString out_dir = _inputExtractPath();
    if (!out_dir.isEmpty())
        _arcModel.extractArchive(out_dir);
}

void MainWindow::slotExtractSelection()
{
    //TODO: fill stub
}

void MainWindow::slotProperties()
{
    //TODO: fill stub
}

void MainWindow::slotSaveAs()
{
    //TODO: fill stub
}

void MainWindow::slotFileOpen()
{
    //TODO: fill stub
}

void MainWindow::slotDelete()
{
    //TODO: fill stub
}

void MainWindow::slotAddFiles()
{
    //TODO: fill stub
}

QString MainWindow::_inputExtractPath()
{
    return QFileDialog::getExistingDirectory(0, "Extract To...");
}

QString MainWindow::_inputSavePath()
{
    //TODO: fill stub
}
