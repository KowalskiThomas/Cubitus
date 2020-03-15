#include "file.h"

File::File(FileId id, FileName fileName)
    : id(id), fileName(fileName)
{

}

FilePointer File::fromJson(const QJsonObject &obj) {
    return FilePointer(new File(
                           obj["fileId"].toString(),
                           obj["fileName"].toString()
                           ));
}
