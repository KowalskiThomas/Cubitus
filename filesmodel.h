#pragma once

#include "common_include.h"
#include "bucket.h"
#include "file.h"

class B2;

class FilesModel : public QObject {
    Q_OBJECT

    using FilesMap = QHash<QString, QHash<QString, FilePointer>>;

    B2* b2 = nullptr;
    FilesMap filesMap_;
    QVector<BucketPointer> buckets_;
    QVector<FilePointer> files_;

public:
    explicit FilesModel(B2*, QObject* parent);

    void init();

    void onApiConnected();
    void onBucketsReceived(QVector<BucketPointer>);
    void onFilesReceived(QVector<FilePointer>);

    [[nodiscard]] const FilesMap& files() const {
        return filesMap_;
    }

    [[nodiscard]] const BucketPointer& bucket(BucketId id) const {
        return *std::find_if(buckets_.begin(), buckets_.end(), [id](BucketPointer p) { return p->id == id; });
    }

    void getFiles(const BucketName& name);

signals:
    void filesReceived(QVector<FilePointer>);
    void bucketsReceived(QVector<BucketPointer>);

};
