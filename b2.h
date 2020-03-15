#pragma once

#include <QObject>
#include <QDebug>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "bucket.h"
#include "file.h"

class NetworkAccessManager;
class FileLister;
class B2;

class FileLister : public QObject {
    Q_OBJECT

    B2* b2;
    FileName nextFileName;
    BucketId bucketId;
    QString prefix;

    QVector<FilePointer> files;
public:
    FileLister(B2* b2, BucketId bucketId, QString prefix)
        : b2(b2), bucketId(bucketId), prefix(prefix)
    {

    }

    void onPartialFileListReceived(QNetworkReply* rep);

    void get();

signals:
    void filesReceived(QVector<FilePointer>);
};

class B2 : public QObject
{
    Q_OBJECT
public:
    enum class DeleteMode {
        DeleteLatestVersion = false,
        DeleteAllVersions = true
    };

private:
    friend FileLister;

    NetworkAccessManager* nam_;

    void onResponse(QNetworkReply*);
    void onAuthenticationResponse(QNetworkReply*);
    void onBucketsReceived(QNetworkReply*);
    void onFileCopied(QNetworkReply*);
    void onFileDeleted(QNetworkReply*, FilePointer, DeleteMode);

    static QString getJson(QJsonObject);
    static QString getJson(QPair<QString, QString>);

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

    void copyFile(FilePointer, FileName destination);
    void deleteFile(FilePointer, DeleteMode = DeleteMode::DeleteLatestVersion);

signals:
    void apiConnected();
    void authenticationFailed();

    void bucketsReceived(QVector<BucketPointer>);
    void filesReceived(QVector<FilePointer>);
    void fileCopied(FilePointer);
    void fileDeleted(FilePointer);
};
