#pragma once

#include <QSharedPointer>
#include <QJsonObject>

using BucketId = QString;

class File;
using FilePointer = QSharedPointer<File>;

using FileName = QString;
using FileId = QString;
using FileSize = size_t;

class File
{
public:
    BucketId bucketId;
    FileId id;
    FileName fileName;
    FileSize fileSize;

public:
    File(BucketId, FileId id, FileName fileName, FileSize fileSize);

    [[nodiscard]] static FilePointer fromJson(const QJsonObject& obj);

    [[nodiscard]] QString fileNameWithoutParentFolder() const;
};
