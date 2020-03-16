#include "b2test.h"

#include "b2.h"


B2Test::B2Test(B2* b2, QObject* parent)
    : QObject(parent), b2(b2)
{
}

void B2Test::test() {
    connect(b2, &B2::apiConnected, this, [this]() {
        b2->getBuckets();
    });

    connect(b2, &B2::bucketsReceived, this, [&](QVector<BucketPointer> buckets) {
        for(auto b : buckets) {
            if (b->name.contains("test", Qt::CaseSensitivity::CaseInsensitive)) {
                bucket = b;
                qInfo() << b->name;
                b2->getFiles(bucket);
                break;
            }
        }
    });

    connect(b2, &B2::filesReceived, this, [&](QVector<FilePointer> files) {
        for(auto f: files) {
            if (!firstFile) {
                firstFile = f;
                b2->copyFile(firstFile, QStringLiteral("Copie de %1.db").arg(firstFile->id));
            }
            else if (!secondFile) {
                secondFile = f;
                b2->renameFile(secondFile, QStringLiteral("Moved %1").arg(secondFile->fileName));
            }

            qInfo() << '\t' << f->fileName;
        }
    });

    connect(b2, &B2::fileCopied, this, [&](FilePointer f) {
        qInfo() << "File copied to" << f->fileName;
        b2->deleteFile(f);
    });

    connect(b2, &B2::fileDeleted, this, [&](FilePointer originalFile) {
        qInfo() << "File" << originalFile->fileName << "deleted";
    });

    connect(b2, &B2::fileRenamed, this, [&](FilePointer from, FilePointer to) {
        qInfo() << "Renamed" << from->fileName << "to" << to->fileName;
    });
}