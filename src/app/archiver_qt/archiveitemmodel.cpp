#include <QFileIconProvider>
#include <QDebug>

#include "archiveitemmodel.h"

enum {
    NUM_COLS = 3, /* name, size, date */
};

ArchiveItemModel::ArchiveItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void ArchiveItemModel::populateModel(std::vector<ArchiveEntry> entries)
{
    beginResetModel();
    _tree.populateTree(entries);
    endResetModel();
//    emit modelReset();

}

void ArchiveItemModel::openArchive(QString filePath)
{
    if (_archive.openArchive(filePath)) {
        std::vector<ArchiveEntry> entries;
        bool success;

        _archive.readAllEntries(entries, success);
        _archive.syncArchive();
        qDebug() << "synced";
        if (success) populateModel(entries);
    }
}

QModelIndex ArchiveItemModel::index(int row, int column, const QModelIndex &parent) const
{
    ArchiveTree::Node parent_node;

    if (parent.isValid()) {
        parent_node = _tree.lookup(parent.internalId());
    } else {
        /* assume that an invalid parent means that the parent is the root node */
        parent_node = _tree.getRootNode();
    }


    if (column < parent_node.children.size())
        return createIndex(row, column, parent_node.children.at(row));
    else
        /* return empty/invalid QModelIndex */
        return QModelIndex();
}

QModelIndex ArchiveItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ArchiveTree::Node parent_node = _tree.lookupParent(index.internalId());

    if (parent_node.isRoot)
        return QModelIndex();

    return createIndex(parent_node.sortedPosition, index.column(), parent_node.index);
}

int ArchiveItemModel::rowCount(const QModelIndex &parent) const
{
    ArchiveTree::Node parent_node;

    if (parent.isValid()) {
        parent_node = _tree.lookup(parent.internalId());
    } else {
        /* assume that an invalid parent means that the parent is the root node */
        parent_node = _tree.getRootNode();
    }

    return parent_node.children.size();
}

int ArchiveItemModel::columnCount(const QModelIndex &parent) const
{
    return NUM_COLS;
}

QVariant ArchiveItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        ArchiveTree::Node node = _tree.lookup(index.internalId());
        switch (index.column()) {
        case 0:
            return node.entry.name;

        case 1:
            return (qulonglong) node.entry.size;

        case 2:
            return ""; //TODO: add date to ArchiveEntry or remove column
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            ArchiveTree::Node node = _tree.lookup(index.internalId());
            QFileIconProvider p;

            if (node.entry.type == ENTRY_TYPE_DIR)
                return p.icon(QFileIconProvider::Folder);
            else
                return p.icon(QFileIconProvider::File);
        }
    }

    return QVariant();
}

QVariant ArchiveItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Name");
            case 1: return tr("Size");
            case 2: return tr("Date");
        }
    }

    return QVariant();
}

Qt::ItemFlags ArchiveItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsUserTristate;
}
