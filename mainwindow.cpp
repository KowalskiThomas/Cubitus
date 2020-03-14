#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "b2.h"
#include "config.h"

MainWindow::MainWindow(B2* b2, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    b2->connectApi(key_id, key);

    connect(b2, &B2::apiConnected, this, [b2]() {
        b2->getBuckets();
    });

    connect(b2, &B2::bucketsReceived, this, [b2](QVector<BucketPointer> buckets) {
        for(auto b : buckets) {
            qInfo() << b->name;
            b2->getFiles(b);
            break;
        }
    });

    connect(b2, &B2::filesReceived, this, [b2](QVector<FilePointer> files) {
       for(auto f: files) {
           qInfo() << f->fileName;
       }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

