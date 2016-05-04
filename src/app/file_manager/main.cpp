#include "mainwindow.h"
#include "genodefs.h"
#include <QApplication>

Q_DECLARE_METATYPE(GenodeFS*)
Q_DECLARE_METATYPE(NodeHandle)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    qRegisterMetaType<GenodeFS*>();
    qRegisterMetaType<NodeHandle>();
    MainWindow w;

	return a.exec();
}
