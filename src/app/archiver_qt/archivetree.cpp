#include "archivetree.h"

ArchiveTree::ArchiveTree()
{
    /* initialization and re-initialization require the same steps */
    clear();
}

ArchiveTree::Node ArchiveTree::getRootNode() const
{
    return _nodes.at(ROOT_NODE_INDEX);
}

ArchiveTree::Node ArchiveTree::lookup(quintptr index) const
{
    return _nodes.at(index);
}

ArchiveTree::Node ArchiveTree::lookupParent(Node child) const
{
    return lookup(child.parentIndex);
}

ArchiveTree::Node ArchiveTree::lookupParent(quintptr index) const
{
    return lookupParent(lookup(index));
}

void ArchiveTree::populateTree(std::vector<ArchiveEntry> entries)
{
    for (ArchiveEntry ent : entries) {
        createDescendant(ROOT_NODE_INDEX, ent);
    }
}

void ArchiveTree::clear()
{
    _nodes.clear();

    Node root;
    root.index = ROOT_NODE_INDEX;
    root.parentIndex = -1; /* wraps around to highest possible value; looking up parent should always throw error */
    root.sortedPosition = 0;
    root.isRoot = true;

    _nodes.push_back(root);
}

void ArchiveTree::sort()
{
    sortChildren(ROOT_NODE_INDEX);
}

void ArchiveTree::sortChildren(quintptr parent)
{
    //TODO: implement!
    // not necessary for basic functionality
    /* std::sort() implementation should be fairly efficient, but we might want to switch to merge sort for large archive directories */
}

quintptr ArchiveTree::lookupChild(Node parent, QString name) const
{
    for (quintptr child_ptr : parent.children) {
        if (_nodes.at(child_ptr).entry.name == name)
            return child_ptr;
    }

    /* lookup failed, so return 0 (root node) */
    return 0;
}

quintptr ArchiveTree::createChild(quintptr parent, ArchiveEntry entry)
{
    const Node parent_node = _nodes.at(parent);

    /* ensure that parent_node is a directory; only directories can have children */
    if (parent_node.entry.type != ENTRY_TYPE_DIR)
        return -1; /* if caller needs to use the child node, don't fail silently */

    /* verify we aren't creating a duplicate entry */
    quintptr lookup_result = lookupChild(parent_node, entry.name);
    if (lookup_result)
        /* duplicate entry; return pre-existing entry rather than creating one */
        return lookup_result;

    Node child;
    child.entry = entry;
    child.index = _nextIndex++;
    child.parentIndex = parent;
    child.sortedPosition = parent_node.children.size();

    _nodes.push_back(child);
    _nodes.at(parent).children.push_back(child.index);

    return child.index;
}

void ArchiveTree::createDescendant(quintptr parent, ArchiveEntry entry)
{
    const Node parent_node = _nodes.at(parent);
    QString subpath = entry.fullPath.mid(parent_node.entry.fullPath.length());

    /* remove leading slash if it exists */
    if (subpath.left(1) == "/")
        subpath = subpath.mid(1);

    int slash_pos = subpath.indexOf('/');

    if (slash_pos >= 0) {
        /* we have a slash, so create recursively */
        QString subdir = subpath.left(slash_pos);
        QString subdir_path;
        if (parent_node.isRoot)
            subdir_path = subdir;
        else
            subdir_path = parent_node.entry.fullPath + "/" + subdir;
        ArchiveEntry subdir_ent(subdir_path, ENTRY_TYPE_DIR, 0);

        /* if subdir doesn't exist, create it; otherwise just look it up */
        quintptr subdir_node = createChild(parent, subdir_ent);
        /* continue recursively creating the node for the given entry */
        createDescendant(subdir_node, entry);
    } else {
        /* leaf node; probably file */
        createChild(parent, entry);
    }
}
