#pragma once

#include <QObject>
#include <QDebug>
#include <QNetworkReply>

#include "bucket.h"
#include "file.h"

class NetworkAccessManager;

class B2 : public QObject
{
    Q_OBJECT

    NetworkAccessManager* nam_;

    void onResponse(QNetworkReply*);
    void onAuthenticationResponse(QNetworkReply*);
    void onBucketsReceived(QNetworkReply*);
    void onPartialFileListReceived(BucketId, size_t c, QNetworkReply*);

    void getFiles(BucketId, size_t requestId, QString);

    template<typename T>
    std::function<void(QNetworkReply*)> getHandler(T meth);

    QString accountId;
    QString token;
    QString apiUrl;

public:
    B2(QObject* parent = nullptr);

    void connectApi(QString keyId, QString key);
    void getBuckets();
    void getFiles(BucketPointer, QString prefix = QString());

signals:
    void apiConnected();
    void authenticationFailed();

    void bucketsReceived(QVector<BucketPointer>);
    void filesReceived(QVector<FilePointer>);
};
