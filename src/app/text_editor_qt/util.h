#ifndef UTIL_H
#define UTIL_H

#include <QString>

class Util
{
public:
    static QString openFile(QString name = QString()); /* creates dialog and returns (virtual) file path */
    static QString saveFileAs(QString name = QString()); /* creates dialog and returns (virtual) file path */
};

#endif // UTIL_H
