#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    new MainWindow; /* automatically takes care of itself */

    return a.exec();
}
