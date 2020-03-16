#pragma once

#include <QSharedPointer>
#include <QJsonObject>

class File;
using FilePointer = QSharedPointer<File>;

using FileName = QString;
using FileId = QString;
using FileSize = size_t;

class File
{
public:
    FileId id;
    FileName fileName;
    FileSize fileSize;

public:
    File(FileId id, FileName fileName, FileSize fileSize);

    [[nodiscard]] static FilePointer fromJson(const QJsonObject& obj);

    [[nodiscard]] QString fileNameWithoutParentFolder() const;
};
