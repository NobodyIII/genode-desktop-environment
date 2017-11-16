#ifndef ARCHIVEITEMMODEL_H
#define ARCHIVEITEMMODEL_H

#include <QAbstractItemModel>

#include "archivetree.h"
#include "asyncarchive.h"

//TODO: add extract, add, and delete functionality

class ArchiveItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveItemModel(QObject *parent = 0);

    void populateModel(std::vector<ArchiveEntry> entries);
    void openArchive(QString filePath);
    void closeArchive();
    void newArchive(QString filePath);
    void extractArchive(QString outDir);

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    ArchiveTree _tree;
    AsyncArchive _archive;
};

#endif // ARCHIVEITEMMODEL_H
