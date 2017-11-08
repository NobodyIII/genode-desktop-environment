#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <memory>
#include <vector>

#include <QString>
#include <QThread>
#include <QMutex>

#include <archive.h>

#include "archiveentry.h"

/* General design notes:
 * 1. The archive is reopened every time it is read or modified, to handle the sequential nature of libarchive.
 * 2. All long functions should be run asynchronously to keep the UI from hanging.
 * 3. libarchive doesn't generally allow appending files to an archive, so we need to rebuild the whole archive
 *    every time a file is added or removed. Consequently, we are delaying write implementation.
 */

class AsyncArchive: public QObject
{
    Q_OBJECT

public:
    AsyncArchive();

    bool openArchive(QString archivePath);
    bool createArchive(QString archivePath);
    void closeArchive();

    bool canWrite();
    bool isReady(); /* Have all asynchronous operations have completed? */
    bool isValid(); /* Is a valid (existing and readable) archive selected? */

    /* These functions access the archive asynchronously. */

    void readAllEntries(std::vector<ArchiveEntry> &result, bool &success);
    void addFiles(QStringList filePaths, bool &success);
    void deleteFiles(QStringList filePaths, bool &success); /* This requires copying the whole archive! */
    void extractFiles(QString extractPath, QStringList filePaths, bool &success);
    void extractAll(QString extractPath, bool &success);

    /* This function is used to ensure that all asynchronous operations have completed. */
    void syncArchive();

signals:
    void progressUpdate(unsigned percentComplete);
    void operationsComplete();

private:
    Q_INVOKABLE void _readAllEntries(void *result_ptr, void *success_ptr);
    Q_INVOKABLE void _addFiles(QStringList filePaths, void *success_ptr);
    Q_INVOKABLE void _deleteFiles(QStringList filePaths, void *success_ptr);
    Q_INVOKABLE void _extractFiles(QString extractPath, QStringList filePaths, void *success_ptr);
    Q_INVOKABLE void _extractAll(QString extractPath, void *success_ptr);

    enum {
        ARCHIVE_BLOCK_SIZE = 10240,
    };

    QString _archivePath;
    QThread _thread;
    QMutex _waitMutex;
    QMutex _queueMutex;
    unsigned _queueLength = 0;

    bool _canWrite = false;
    bool _isReady = true;
    bool _isValid = false;

    void _beginOperation();
    void _endOperation();
};


#endif // ARCHIVE_H
