#include <QOBject>

#include "bucket.h"
#include "file.h"

class B2;

class B2Test : public QObject {
    Q_OBJECT

    B2* b2;
    BucketPointer bucket;
    FilePointer firstFile;
    FilePointer secondFile;

public:
    explicit B2Test(B2*, QObject* parent = nullptr);
    void test();
};
