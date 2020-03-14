#include "file.h"

File::File(FileName fileName)
    : fileName(fileName)
{

}

FilePointer File::fromJson(const QJsonObject &obj) {
    return FilePointer(new File(
                           obj["fileName"].toString()
                           ));
}
