#ifndef FILESTREEMODEL_H
#define FILESTREEMODEL_H

#include <QAbstractItemModel>

#include "filesmodel.h"
#include "filetreeitem.h"

class FilesTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    FilesModel* filesModel;
    QMap<QString, FileTreeItem *> folders;
    QMap<BucketId, FileTreeItem *> bucketItems = {{"", rootItem}};

public:
    explicit FilesTreeModel(FilesModel* filesModel_, QObject *parent = nullptr);
    ~FilesTreeModel() override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void changeRoot(const QModelIndex&);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    FileTreeItem *rootItem;

    FileTreeItem *getParentForFileName(const BucketPointer& bucket, const FileName& fileName);
};

#endif // FILESTREEMODEL_H
