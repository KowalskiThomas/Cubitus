#include "filesmodel.h"

#include <utility>

#include "b2.h"
#include "config.h"

FilesModel::FilesModel(B2* b2, QObject* parent)
    : b2(b2)
{
    connect(b2, &B2::apiConnected, this, &FilesModel::onApiConnected);
    connect(b2, &B2::bucketsReceived, this, &FilesModel::onBucketsReceived);
    connect(b2, &B2::filesReceived, this, &FilesModel::onFilesReceived);
}

void FilesModel::init() {
    b2->connectApi(key_id, key);
}

void FilesModel::onApiConnected() {
    b2->getBuckets();
}

void FilesModel::onBucketsReceived(QVector<BucketPointer> buckets) {
    buckets_ = std::move(buckets);
    qDebug() << buckets_.size();
    auto testBucket = std::find_if(buckets_.begin(), buckets_.end(), [](BucketPointer b) {
        return b->name.contains("test", Qt::CaseInsensitive);
    });
    if (testBucket != buckets_.end())
        b2->getFiles(*testBucket);
    else
        qDebug() << "Can't find bucket";
}

void FilesModel::onFilesReceived(QVector<FilePointer> files) {
    emit filesReceived(std::move(files));
}