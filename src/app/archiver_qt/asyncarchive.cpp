#include <QFile>
#include <QDebug>

extern "C" {
    #include <archive.h>
    #include <archive_entry.h>
}

#include "asyncarchive.h"

AsyncArchive::AsyncArchive()
{
    moveToThread(&_thread);
    _thread.start();
}

bool AsyncArchive::openArchive(QString archivePath)
{
    syncArchive();

    QFile archive_file(archivePath);

    if (archive_file.exists()) {
        _archivePath = archivePath;
        _canWrite = archive_file.isWritable();
        _isReady = true;
        _isValid = true;
        return true;
    }

    /* archive does not exist or is not readable */
    return false;
}

bool AsyncArchive::createArchive(QString archivePath)
{
    syncArchive();

    archive *arc = archive_write_new();
    int status = archive_write_open_filename(arc, archivePath.toStdString().c_str());
    archive_write_free(arc);

    bool success = (status == ARCHIVE_OK);
    _canWrite = success;
    _isReady = success;
    _isValid = success;
    return success;
}

void AsyncArchive::closeArchive()
{
    syncArchive();
    _isValid = false;
    _canWrite = false;
}

bool AsyncArchive::canWrite()
{
    return _canWrite;
}

bool AsyncArchive::isReady()
{
    return _isReady;
}

bool AsyncArchive::isValid()
{
    return _isValid;
}

void AsyncArchive::readAllEntries(std::vector<ArchiveEntry> &result, bool &success)
{
    /* this function simply converts the function call to an asyncronous call */
    _beginOperation();
    result.clear(); /* clear on main thread to avoid issues with deleting entries */
    QMetaObject::invokeMethod(this, "_readAllEntries", Qt::QueuedConnection,
                              Q_ARG(void *, &result), Q_ARG(void *, &success));
}

void AsyncArchive::addFiles(QStringList filePaths, bool &success)
{
    /* this function simply converts the function call to an asyncronous call */
    _beginOperation();
    QMetaObject::invokeMethod(this, "_addFiles", Qt::QueuedConnection,
                              Q_ARG(QStringList, filePaths), Q_ARG(void *, &success));
}

void AsyncArchive::deleteFiles(QStringList filePaths, bool &success)
{
    /* this function simply converts the function call to an asyncronous call */
    _beginOperation();
    QMetaObject::invokeMethod(this, "_deleteFiles", Qt::QueuedConnection,
                              Q_ARG(QStringList, filePaths), Q_ARG(void *, &success));
}

void AsyncArchive::extractFiles(QString extractPath, QStringList filePaths, bool &success)
{
    /* this function simply converts the function call to an asyncronous call */
    _beginOperation();
    QMetaObject::invokeMethod(this, "_extractFiles", Qt::QueuedConnection,
                              Q_ARG(QString, extractPath), Q_ARG(QStringList, filePaths), Q_ARG(void *, &success));
}

void AsyncArchive::extractAll(QString extractPath, bool &success)
{
    /* this function simply converts the function call to an asyncronous call */
    _beginOperation();
    QMetaObject::invokeMethod(this, "_extractAll", Qt::QueuedConnection,
                              Q_ARG(QString, extractPath), Q_ARG(void *, &success));
}

void AsyncArchive::syncArchive()
{
    /* just wait for _waitMutex to become available, then return */
    QMutexLocker locker(&_waitMutex);
}

void AsyncArchive::_readAllEntries(void *result_ptr, void *success_ptr)
{
    std::vector<ArchiveEntry> &result = *(static_cast<std::vector<ArchiveEntry> *>(result_ptr));
    bool &success = *(static_cast<bool *>(success_ptr));

    int status;

    archive *arc = archive_read_new();
    archive_read_support_format_all(arc);
    archive_read_support_filter_all(arc);
    /* open archive for reading */
    status = archive_read_open_filename(arc, _archivePath.toStdString().c_str(), ARCHIVE_BLOCK_SIZE);
    if (status != ARCHIVE_OK) {
        /* failure, so cleanup and return */
        qDebug() << archive_error_string(arc);
        archive_read_free(arc);
        success = false;
        _endOperation();
        return;
    }

    /* iterate over headers/entries */
    archive_entry *entry;
    while (archive_read_next_header(arc, &entry) == ARCHIVE_OK) {

        EntryType type;
        switch (archive_entry_filetype(entry)) {
        case AE_IFREG:
            type = ENTRY_TYPE_STDFILE;
            break;

        case AE_IFDIR:
            type = ENTRY_TYPE_DIR;
            break;

        case AE_IFLNK:
            type = ENTRY_TYPE_SYMLINK;
            //TODO: store link target
            break;

        default:
            type = ENTRY_TYPE_OTHER;
            break;
        }

        result.emplace_back(archive_entry_pathname(entry), type, archive_entry_size(entry));
    }

    /* success; cleanup and return */
    archive_read_free(arc);
    success = true;

    _endOperation();
    qDebug() << "finished";
}

void AsyncArchive::_addFiles(QStringList filePaths, void *success)
{
    //TODO: fill stub
}

void AsyncArchive::_deleteFiles(QStringList filePaths, void *success)
{
    //TODO: fill stub
}

void AsyncArchive::_extractFiles(QString extractPath, QStringList filePaths, void *success_ptr)
{
    //TODO: fill stub
}

void AsyncArchive::_extractAll(QString extractPath, void *success_ptr)
{
    //TODO: fill stub
}

void AsyncArchive::_beginOperation()
{
    QMutexLocker locker(&_queueMutex);
    if (! _queueLength++) {
        /* transitioning from ready to busy */
        _waitMutex.lock();
        _isReady = false;
    }
}

void AsyncArchive::_endOperation()
{
    QMutexLocker locker(&_queueMutex);
    --_queueLength;

    if (_queueLength == 0) {
        _isReady = true;
        _waitMutex.unlock();
        emit operationsComplete();
    }
}
