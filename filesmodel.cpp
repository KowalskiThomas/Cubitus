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
    for(const auto& bucket : buckets_)
        if (!filesMap_.contains(bucket->id))
            filesMap_[bucket->id] = QHash<QString, FilePointer>();

    emit bucketsReceived(buckets_);
}

void FilesModel::getFiles(const BucketName& name) {
    qInfo() << "Gathering file for bucket" << name;
    auto bucketIt = std::find_if(buckets_.begin(), buckets_.end(), [this, name](BucketPointer p) {
        return p->name == name;
    });
    if (bucketIt == buckets_.end()) {
        qWarning() << "Could not find bucket" << name;
        return;
    }
    b2->getFiles(*bucketIt);
}

void FilesModel::onFilesReceived(QVector<FilePointer> files) {
    emit filesReceived(std::move(files));
}
