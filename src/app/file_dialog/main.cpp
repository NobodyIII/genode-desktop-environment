#include <QApplication>

#include "filedialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //TODO: get options from config

    FileDialog dialog("");
    if (dialog.exec())
        qDebug() << dialog.selectedFiles();

    //TODO: report file selection

    return 0;
}
