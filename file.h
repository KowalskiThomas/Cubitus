#pragma once

#include <QSharedPointer>
#include <QJsonObject>

class File;
using FilePointer = QSharedPointer<File>;

using FileName = QString;

class File
{
public:
    FileName fileName;
public:
    File(FileName fileName);

    [[nodiscard]] static FilePointer fromJson(const QJsonObject& obj);
};
