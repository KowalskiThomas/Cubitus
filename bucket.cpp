#include "bucket.h"

Bucket::Bucket(BucketId id, BucketName name)
    : id(std::move(id)), name(std::move(name))
{
}

BucketPointer Bucket::fromJson(const QJsonObject &obj) {
    return BucketPointer(
                new Bucket(
                    obj["bucketId"].toString(),
                    obj["bucketName"].toString()
                )
            );
}
