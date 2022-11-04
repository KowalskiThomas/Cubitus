#pragma once

#include <QVector>
#include <QVariant>

enum class FileTreeItemType {
    File = 0,
    Folder,
    Bucket,
    Root
};

using FileSize = size_t;

class FileTreeItem {
private:
    QVector<FileTreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    FileTreeItem *m_parentItem;
    FileTreeItemType m_itemType;

public:
    explicit FileTreeItem(QVector<QVariant> data, FileTreeItemType itemType, FileTreeItem *parentItem = nullptr);
    ~FileTreeItem();

    void appendChild(FileTreeItem *child);

    QString humanSize(FileSize) const;

    FileTreeItem *child(int row);
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    [[nodiscard]] int row() const;
    FileTreeItem *parentItem();

    [[nodiscard]] bool isFolder() const { return m_itemType == FileTreeItemType::Folder; }
    [[nodiscard]] bool isBucket() const { return m_itemType == FileTreeItemType::Bucket; }
    [[nodiscard]] bool isFile() const { return m_itemType == FileTreeItemType::File; }
};
