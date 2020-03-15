#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "b2.h"
#include "config.h"

static BucketPointer bucket;
static FilePointer firstFile;
static FilePointer secondFile;

MainWindow::MainWindow(B2* b2_, QWidget *parent)
    : b2(b2_),
      QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    b2->connectApi(key_id, key);

    connect(b2, &B2::apiConnected, this, [this]() {
        b2->getBuckets();
    });

    connect(b2, &B2::bucketsReceived, this, [&](QVector<BucketPointer> buckets) {
        for(auto b : buckets) {
            if (b->name.contains("test", Qt::CaseSensitivity::CaseInsensitive)) {
                bucket = b;
                qInfo() << b->name;
                b2->getFiles(b);
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

MainWindow::~MainWindow()
{
    delete ui;
}

