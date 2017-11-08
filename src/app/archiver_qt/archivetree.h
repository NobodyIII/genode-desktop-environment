#ifndef ARCHIVETREE_H
#define ARCHIVETREE_H

#include <vector>
#include <QList>

#include "archiveentry.h"

/* General design notes:
 * 1. All nodes are referenced by index, NOT by reference or pointer.
 *    _nodes (std::vector) sometimes reallocates memory and moves all of the Nodes to different memory locations.
 *    Because of this, we should NEVER use references; we should lookup each node each time we modify it.
 */

class ArchiveTree
{
public:
    struct Node
    {
        quintptr index;
        quintptr parentIndex;
        int sortedPosition;
        ArchiveEntry entry;
        std::vector<quintptr> children;
        bool isRoot = false;
    };

    ArchiveTree();

    Node lookup(quintptr index) const;
    Node lookup(QString path) const;
    void populateTree(std::vector<ArchiveEntry> entries);
    void clear();
    void sort();

    Node getRootNode() const;
    Node lookupParent(Node child) const;
    Node lookupParent(quintptr index) const;
    quintptr lookupChild(Node parent, QString name) const; /* returns 0 (root node) if not found */
    void sortChildren(quintptr parent); /* recursively sort vector parent.children then update Nodes' sortedIndex variables */
    quintptr createChild(quintptr parent, ArchiveEntry entry);
    void createDescendant(quintptr parent, ArchiveEntry entry);

private:
    enum {
        ROOT_NODE_INDEX = 0,
    };

    std::vector<Node> _nodes;
    quintptr _nextIndex = 1; /* next after root node */
};

#endif // ARCHIVETREE_H
