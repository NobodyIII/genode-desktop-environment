#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFile>

class Utils
{
public:
    static bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);
};

#endif // UTILS_H
