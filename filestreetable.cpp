#include "filestreetable.h"

#include "filestreemodel.h"

FilesTreeTable::FilesTreeTable(QWidget* parent)
    : QTreeView(parent)
{
    connect(this, &FilesTreeTable::clicked, this, &FilesTreeTable::onClick);
    connect(this, &FilesTreeTable::doubleClicked, this, &FilesTreeTable::onDoubleClick);


    installEventFilter(this);
}

bool FilesTreeTable::eventFilter(QObject* obj, QEvent* event) {
    /*
    if(event->type() == QEvent::MouseButtonDblClick) {
        qDebug() << "coucouo";
    } else if (event->type() == QEvent::MouseButtonPress) {
        qDebug() << "pres";
    }
    */

    return false;
}

void FilesTreeTable::setModel(QAbstractItemModel* model)
{
    QTreeView::setModel(model);
    model_ = qobject_cast<FilesTreeModel*>(model);

    setSelectionBehavior(SelectionBehavior::SelectRows);

    auto selModel = selectionModel();
    connect(selModel, &QItemSelectionModel::selectionChanged, this, &FilesTreeTable::onSelectionChanged);

}

void FilesTreeTable::onClick(const QModelIndex &) {

}

void FilesTreeTable::onDoubleClick(const QModelIndex& idx) {
    model_->changeRoot(idx);
}

void FilesTreeTable::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
//    for(auto x : selected)
//        qDebug() << x;
//    for(auto x : deselected)
//        qDebug() << x;
}
