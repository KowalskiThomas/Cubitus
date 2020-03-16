#pragma once

#include <QTreeView>
#include <QDebug>
#include <QEvent>


class FilesTreeModel;

class FilesTreeTable : public QTreeView {
    Q_OBJECT

    FilesTreeModel* model_;

    void onClick(const QModelIndex&);
    void onDoubleClick(const QModelIndex&);
    void onSelectionChanged(const QItemSelection&, const QItemSelection&);

    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    explicit FilesTreeTable(QWidget* parent = nullptr);

    void setModel(QAbstractItemModel *model) override;
};
