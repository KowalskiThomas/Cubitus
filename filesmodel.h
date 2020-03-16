#pragma once

#include "common_include.h"
#include "bucket.h"
#include "file.h"

class B2;

class FilesModel : public QObject {
    Q_OBJECT

    B2* b2 = nullptr;
    QVector<BucketPointer> buckets_;
    QVector<FilePointer> files_;

public:
    explicit FilesModel(B2*, QObject* parent);

    void init();

    void onApiConnected();
    void onBucketsReceived(QVector<BucketPointer>);
    void onFilesReceived(QVector<FilePointer>);

signals:
    void filesReceived(QVector<FilePointer>);
};
