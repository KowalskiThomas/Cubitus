#include "file.h"
#include "bucket.h"

#include <utility>

File::File(BucketId bucketId, FileId id, FileName fileName, FileSize fileSize)
    : bucketId(bucketId),
    id(std::move(id)),
    fileName(std::move(fileName)),
    fileSize(fileSize)
{

}

FilePointer File::fromJson(const QJsonObject &obj) {
    return FilePointer(new File(
            obj["bucketId"].toString(),
                           obj["fileId"].toString(),
                           obj["fileName"].toString(),
                           obj["contentLength"].toInt()
                           ));
}

QString File::fileNameWithoutParentFolder() const {
    auto lastSlash = fileName.lastIndexOf('/');
    if (lastSlash == -1)
        return fileName;

    return fileName.mid(lastSlash + 1);
}
