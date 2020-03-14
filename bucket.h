#pragma once

#include <QSharedPointer>
#include <QJsonObject>

class Bucket;
using BucketPointer = QSharedPointer<Bucket>;

using BucketId = QString;
using BucketName = QString;

class Bucket
{
public:
    BucketId id;
    BucketName name;

public:
    Bucket(BucketId id, BucketName name);

    [[nodiscard]] static BucketPointer fromJson(const QJsonObject& obj);
};
