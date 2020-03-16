#pragma once

#include <QVector>
#include <QVariant>

class FileTreeItem {
private:
    QVector<FileTreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    FileTreeItem *m_parentItem;
    bool isFolder_;

public:
    explicit FileTreeItem(QVector<QVariant> data, bool isFolder, FileTreeItem *parentItem = nullptr);
    ~FileTreeItem();

    void appendChild(FileTreeItem *child);

    FileTreeItem *child(int row);
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    [[nodiscard]] int row() const;
    FileTreeItem *parentItem();

    [[nodiscard]] bool isFolder() const { return isFolder_; }
};
