#ifndef FSTREENODE_H
#define FSTREENODE_H

#include "fsnode.h"

struct FSTreeNode
{
    QString name;
    quint64 size;
    FSNode::TYPE type;
    QVector<FSTreeNode> children;
    FSTreeNode() { }
    FSTreeNode(QString name, quint64 size, FSNode::TYPE type) {
        this->name=name;
        this->size=size;
        this->type=type;
    }
};

#endif // FSTREENODE_H

