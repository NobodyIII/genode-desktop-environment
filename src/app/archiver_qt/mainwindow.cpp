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

    qDebug() << style()->standardIcon(QStyle::SP_DirIcon).availableSizes();
    QPixmap *pm = new QPixmap(style()->standardIcon(QStyle::SP_DirIcon).pixmap(QSize(32, 32), QIcon::Normal, QIcon::On));
//    pm->fill(Qt::transparent);
    QPainter painter(pm);
//    painter.drawPixmap(pm->rect(), style()->standardIcon(QStyle::SP_DirOpenIcon, &opt).pixmap(opt.iconSize));
//    qDebug() << iconSize();
    QPoint pos((iconSize().width() - 16) / 2, 0);
    painter.drawPixmap(pos, style()->standardIcon(QStyle::SP_ArrowUp).pixmap(QSize(16, 16)));

    /* set icons */
    ui->actionAddFiles->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    ui->actionClose->setIcon(style()->standardIcon((QStyle::SP_DialogCloseButton)));
    ui->actionDelete->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->actionExtract->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->actionExtractAll->setIcon(*pm);
    ui->actionFileOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionFileOpenWith->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionNew->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionProperties->setIcon(style()->standardIcon(QStyle::SP_FileDialogInfoView));
    ui->actionSaveAs->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    //TODO: handle more UI actions!
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(slotOpen()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSave()
{

}

void MainWindow::slotSaveAs()
{

}

void MainWindow::slotOpen()
{
    QString file_path = QFileDialog::getOpenFileName();
    _arcModel.openArchive(file_path);
}
