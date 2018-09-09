#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include <functional>

#include "fsmodel.h"
#include "utils.h"

void FSModel::updateNodes(QList<QFileInfo> nodes)
{
    beginResetModel();
    this->nodes=nodes;
    endResetModel();
}

FSModel::FSModel()
{
    go("/");
}

QModelIndex FSModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row,column,(quintptr)0);
}

int FSModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodes.size();
}

int FSModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : 3;
}

QVariant FSModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    QFileInfo node=nodes.value(index.row());
    if(role == Qt::DisplayRole)
    {
        switch(index.column()) {
        case 0:
            return node.fileName();
        case 1:
            /* reserved for file type (file, dir, symlink) */
            return 0;
        case 2:
            return node.size();
            break;
        }
    } else if (role==Qt::DecorationRole) {
        if (index.column()==0) {
            QFileIconProvider p;

            if (node.isDir())
                return p.icon(QFileIconProvider::Folder);
            else
                return p.icon(QFileIconProvider::File);
        }
    } else if (role==Qt::EditRole) {
        return node.fileName();
    } else if (role==Qt::StatusTipRole) {
        return node.fileName();
    } else if (role==Qt::TextAlignmentRole) {
        return Qt::AlignHCenter + Qt::AlignTop;
    }

    return QVariant();
}

QVariant FSModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if(role == Qt::DisplayRole)
    switch(section) {
    case 0: return tr("Name");
    case 1: return tr("Type");
    case 2: return tr("Size");
    default: return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags FSModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex FSModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

QStringList FSModel::mimeTypes() const {
    return QStringList("text/uri-list");
}

Qt::DropActions FSModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

QMimeData *FSModel::mimeData(const QModelIndexList &indices) const {
    QMimeData *data=new QMimeData();
    QList<QUrl> urls;
    foreach (QModelIndex index, indices) {
        if (index.isValid())
            urls << QUrl::fromLocalFile(nodes.value(index.row()).filePath());
    }
    data->setUrls(urls);
    return data;
}

bool FSModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    Q_UNUSED(row);
    Q_UNUSED(column);

    bool ret = true;
    if (!data->hasUrls() || !parent.isValid() || (action!=Qt::MoveAction && action!=Qt::CopyAction) || parent.row()<0) return false;
    QFileInfo newdir=nodes.value(parent.row());
    if (!newdir.isDir() || newdir.absoluteFilePath() == rootItem.absoluteFilePath()) return false;
    foreach (QUrl url, data->urls()) {
        QFileInfo node(url.toLocalFile());
        if (node.exists() && node.filePath() != newdir.filePath()) {
            if (action==Qt::MoveAction)
                ret &= QFile::rename(node.absoluteFilePath(), newdir.absoluteFilePath() + "/" + node.fileName());
            else if (action==Qt::CopyAction)
                 ret &= Utils::copyRecursively(node.absoluteFilePath(), newdir.absoluteFilePath() + "/" + node.fileName());
        }
    }
    refresh();
	return ret;
}

QString FSModel::url_to_string(QUrl url)
{
    return url.toLocalFile();
}

QUrl FSModel::string_to_url(QString str)
{
    return QUrl::fromLocalFile(str);
}

QStringList FSModel::urls_to_strings(QList<QUrl> urls)
{
    QStringList strs;
    foreach (QUrl url, urls) {
        strs << url.toLocalFile();
    }
    return strs;
}

QList<QUrl> FSModel::strings_to_urls(QStringList strs)
{
    QList<QUrl> urls;
    foreach (QString str, strs) {
        urls << string_to_url(str);
    }
    return urls;
}

QString FSModel::getcwd()
{
    return rootItem.absoluteFilePath();
}

void FSModel::up()
{
    go(rootItem.absolutePath());
}

void FSModel::back()
{
    if (hist_pos>0) {
        --hist_pos;
        rootItem.setFile(history[hist_pos]);
        refresh();
    }
}

void FSModel::forward()
{
    if (hist_pos<history.size()-1) {
        ++hist_pos;
        rootItem.setFile(history[hist_pos]);
        refresh();
    }
}

void FSModel::refresh()
{
    updateNodes(QDir(rootItem.absoluteFilePath()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase | QDir::DirsFirst));
}

void FSModel::cut(QModelIndexList indices)
{
    cut_copy_list=indices_to_nodes(indices);
    cutting=true;
}

void FSModel::copy(QModelIndexList indices)
{
    cut_copy_list=indices_to_nodes(indices);
    cutting=false;
}

void FSModel::paste() /* paste into cwd */
{
    foreach (const QFileInfo &node, cut_copy_list) {
        if (node.exists()) {
            if (cutting)
                QFile::rename(node.absoluteFilePath(), rootItem.absoluteFilePath() + "/" + node.fileName());
            else
                Utils::copyRecursively(node.absoluteFilePath(), rootItem.absoluteFilePath() + "/" + node.fileName());
        }
    }
    refresh();
}

void FSModel::del(QModelIndexList indices)
{
    foreach (QModelIndex index,indices) {
        if (index.isValid()) {
            QFileInfo node=nodes.value(index.row());
            if (node.exists()) {
                if (node.isDir())
                    QDir(node.absoluteFilePath()).removeRecursively();
                else
                    QFile::remove(node.absoluteFilePath());
            }
        }
    }
    refresh();
}

QModelIndex FSModel::new_folder()
{
    QString new_name="New Folder";
    QDir(rootItem.absoluteFilePath()).mkdir(new_name);
    refresh();
    for (int i = 0; i < nodes.size(); ++i) {
        QFileInfo &node=nodes[i];
        if (node.fileName()==new_name) return index(i,0);
    }
    return QModelIndex();
}

QModelIndex FSModel::new_file()
{
    QString new_name="New File";
    QFile file(rootItem.absoluteFilePath() + "/" + new_name);
    file.open(QFile::WriteOnly);
    file.close();
    refresh();
    for (int i = 0; i < nodes.size(); ++i) {
        QFileInfo &node=nodes[i];
        if (node.fileName()==new_name) return index(i,0);
    }
    return QModelIndex();
}

void FSModel::go(QString path)
{
    QString prev_path=rootItem.absoluteFilePath();
    rootItem.setFile(path);
    while (history.size()-1>hist_pos) {
        history.removeLast();
    }
    if (rootItem.absoluteFilePath()!=prev_path) {
        history << rootItem.absoluteFilePath();
        ++hist_pos;
    }
    refresh();
}

void FSModel::open(QModelIndex index)
{
    if (index.isValid()) {
        QFileInfo node=nodes.value(index.row());
        if (node.isDir()) go(node.absoluteFilePath());
    }
}

QList<QFileInfo> FSModel::indices_to_nodes(QModelIndexList indices)
{
    QList<QFileInfo> new_nodes;
    foreach (QModelIndex index, indices) {
        QFileInfo node = nodes.value(index.row());
        if (node.exists()) new_nodes << node; /* copies node to list */
    }
    return new_nodes;
}

bool FSModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    /* rename file */
	bool ret = false;
    QFileInfo node=nodes.value(index.row());
    if (node.exists())
        ret = QFile::rename(node.absoluteFilePath(), rootItem.absoluteFilePath() + "/" + value.toString());
    refresh();
	return ret;
}

void FSModel::setSorting(SortingAttribute attr, int dir) {
    sort_attr=attr;
    sort_dir=dir;
    refresh();
}
