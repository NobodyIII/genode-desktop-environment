#include "mainwindow.h"
#include "genodefs.h"
#include <QApplication>

Q_DECLARE_METATYPE(GenodeFS*)
Q_DECLARE_METATYPE(NodeHandle)

int main(int argc, char *argv[])
{
	qDebug() << "creating QApplication";
	QApplication a(argc, argv);
	qDebug() << "1";
    qRegisterMetaType<GenodeFS*>();
	qDebug() << "2";
    qRegisterMetaType<NodeHandle>();
	qDebug() << "3";
    MainWindow w;

	qDebug() << "QApplication.exec()";
	return a.exec();
}

#ifndef TEST_LINUX

#include <libc/component.h>

/* libc includes */
#include <stdlib.h> /* 'malloc' */
#include <stdlib.h> /* 'exit'   */

extern char **genode_argv;
extern int    genode_argc;

void Libc::Component::construct(Libc::Env &env)
{
// 	char *argv[] = {""};
	GenodeFS::set_env(env);
	Libc::with_libc([&] () { main(genode_argc, genode_argv); });
}

#endif
