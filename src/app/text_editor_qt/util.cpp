#include "util.h"

#include <QFileDialog> //TODO: remove!

QString Util::openFile(QString name)
{
    //TODO: replace with Genode code
    return QFileDialog::getOpenFileName(0, "", name);
}

QString Util::saveFileAs(QString name)
{
    //TODO: replace with Genode code
    return QFileDialog::getSaveFileName(0, "", name);
}
