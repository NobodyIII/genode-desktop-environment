#ifndef ARCHIVEENTRY_H
#define ARCHIVEENTRY_H

#include <cstdint>

#include <QString>
#include <QMetaType>

enum EntryType
{
    ENTRY_TYPE_STDFILE,
    ENTRY_TYPE_SYMLINK,
    ENTRY_TYPE_DIR,
    ENTRY_TYPE_OTHER,
};

struct ArchiveEntry
{
    QString fullPath;
    QString name;
    EntryType type;
    uint64_t size;

    ArchiveEntry():
        type(ENTRY_TYPE_DIR), size(0)
        { }

    ArchiveEntry(QString ent_path, EntryType ent_type, uint64_t ent_size):
        fullPath(ent_path), type(ent_type), size(ent_size)
    {
        if (fullPath.endsWith('/'))
            fullPath.chop(1);

        int slashpos = fullPath.lastIndexOf('/');
        /* at least for now, absolute paths are not allowed, so always remove leading slashes */
        name = fullPath.mid(slashpos + 1);
    }
};

Q_DECLARE_METATYPE(ArchiveEntry)

#endif // ARCHIVEENTRY_H
