#pragma once

#include <QSharedPointer>
#include <QJsonObject>

class File;
using FilePointer = QSharedPointer<File>;

using FileName = QString;
using FileId = QString;

class File
{
public:
    FileId id;
    FileName fileName;

public:
    File(FileId id, FileName fileName);

    [[nodiscard]] static FilePointer fromJson(const QJsonObject& obj);
};
