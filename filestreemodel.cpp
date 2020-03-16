#include "filestreemodel.h"

#include <QDebug>

QVector<FileTreeItem *> items;

FileTreeItem* FilesTreeModel::getParentForFileName(FileName fileName) {
    auto lastSlash = fileName.lastIndexOf('/');
    if (lastSlash == -1)
        return rootItem;

    auto parentFolder = fileName.mid(0, lastSlash);

    FileTreeItem *parentItem = nullptr;
    if (folders.contains(parentFolder)) {
        parentItem = folders[parentFolder];
    } else {
        auto parentParent = getParentForFileName(parentFolder);
        auto newParent = new FileTreeItem({parentFolder}, true, parentParent);
        parentParent->appendChild(newParent);
        parentItem = newParent;
        folders[parentFolder] = parentItem;
    }
    return parentItem;
}

FilesTreeModel::FilesTreeModel(FilesModel *filesModel, QObject *parent)
        : filesModel(filesModel), QAbstractItemModel(parent) {
    rootItem = new FileTreeItem({}, true);
    filesModel->init();


    connect(filesModel, &FilesModel::filesReceived, this, [this](QVector<FilePointer> files) {
        emit layoutAboutToBeChanged();
        for (auto file : files) {
            auto fileName = file->fileName;
            if (fileName.endsWith(".bzEmpty"))
                continue;

            auto parent = getParentForFileName(fileName);
            auto item = new FileTreeItem({file->fileNameWithoutParentFolder(), file->id, QString::number(file->fileSize)}, false, parent);
            parent->appendChild(item);
            items.push_back(item);
        }
        emit layoutChanged();
    });
}

QVariant FilesTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static QVector<QString> sections = {"Name", "ID", "Size"};
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return sections[section];

    return QVariant();
}

bool FilesTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
    if (value != headerData(section, orientation, role)) {
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex FilesTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FileTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileTreeItem *>(parent.internalPointer());

    FileTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex FilesTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<FileTreeItem *>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FilesTreeModel::rowCount(const QModelIndex &parent) const {
    FileTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileTreeItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int FilesTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<FileTreeItem *>(parent.internalPointer())->columnCount();

    return 3;
}

QVariant FilesTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    auto item = static_cast<FileTreeItem *>(index.internalPointer());
    return item->data(index.column());
}

bool FilesTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (data(index, role) != value) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags FilesTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

FilesTreeModel::~FilesTreeModel() {
    delete rootItem;
}

void FilesTreeModel::changeRoot(const QModelIndex &idx) {
    emit layoutAboutToBeChanged();
    auto pointer = static_cast<FileTreeItem *>(idx.internalPointer());
    if (!pointer->isFolder())
        return;
    rootItem = pointer;
    emit layoutChanged();
}

FileTreeItem::FileTreeItem(QVector<QVariant> data, bool isFolder, FileTreeItem *parentItem)
        : m_itemData(std::move(data)), isFolder_(isFolder), m_parentItem(parentItem) {
}

FileTreeItem::~FileTreeItem() {
    qDeleteAll(m_childItems);
}

void FileTreeItem::appendChild(FileTreeItem *child) {
    m_childItems.append(child);
}

FileTreeItem *FileTreeItem::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;

    return m_childItems.at(row);
}

int FileTreeItem::childCount() const {
    return m_childItems.count();
}

int FileTreeItem::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<FileTreeItem *>(this));

    return 0;
}

int FileTreeItem::columnCount() const {
    return m_itemData.count();
}

QVariant FileTreeItem::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();

    return m_itemData.at(column);
}

FileTreeItem *FileTreeItem::parentItem() {
    return m_parentItem;
}


