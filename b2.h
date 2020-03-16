#pragma once

#include "common_include.h"
#include "bucket.h"
#include "file.h"

class NetworkAccessManager;
class FileLister;
class B2;


class OneShotReplyHandler : public QObject {
    Q_OBJECT
public:
    using handler_t = std::function<void(void*)>;

private:
    handler_t handler;

public:
    OneShotReplyHandler(handler_t handler)
        : handler(handler)
    {

    }

    void handle(void* rep) {
        handler(rep);
        deleteLater();
    }
};

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
    void onFileCopied(QNetworkReply*, FilePointer);
    void onFileDeleted(QNetworkReply*, FilePointer, DeleteMode);
    void onFileRenamed(QNetworkReply*, FilePointer);
    void onFileCopiedInternal(FilePointer from, FilePointer to);
    void onFileDeletedInternal(FilePointer);

    QSet<FileId> awaitingCopies;
    QSet<FileId> awaitingDeletions;

    QMap<FileId, QPair<FilePointer, FilePointer>> awaitingMoves;

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
    void renameFile(FilePointer, FileName);

signals:
    void apiConnected();
    void authenticationFailed();

    void bucketsReceived(QVector<BucketPointer>);
    void filesReceived(QVector<FilePointer>);
    void fileCopied(FilePointer from, FilePointer to);
    void fileCopiedInternal(FilePointer from, FilePointer to);
    void fileDeleted(FilePointer);
    void fileDeletedInternal(FilePointer);
    void fileRenamed(FilePointer from, FilePointer to);
};
