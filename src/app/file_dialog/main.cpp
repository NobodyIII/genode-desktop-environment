#include <QApplication>

#include "filedialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //TODO: get options from config

    FileDialog dialog("Open File", FileDialog::DEFAULT, ""); //(0, "Open File", "", "");
    dialog.exec();

    //TODO: report file selection

    return 0;
}
