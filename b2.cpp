#include "b2.h"

#include "networkaccessmanager.h"

template<typename T>
std::function<void(QNetworkReply*)> B2::getHandler(T meth) {
    return std::bind(meth, this, std::placeholders::_1);
}

B2::B2(QObject* parent)
    : QObject(parent),
      nam_(NetworkAccessManager::get())
{
    connect(this, &B2::fileCopiedInternal, this, &B2::onFileCopiedInternal);
    connect(this, &B2::fileDeletedInternal, this, &B2::onFileDeletedInternal);
}

QString B2::getJson(QJsonObject obj) {
    return QJsonDocument(obj).toJson();
}

QString B2::getJson(QPair<QString, QString> s) {
    return getJson(QJsonObject({{s.first, s.second}}));
}

void B2::onAuthenticationResponse(QNetworkReply* rep) {
    auto content = rep->readAll();
    auto doc = QJsonDocument::fromJson(content).object();
    if (doc.contains("authorizationToken")) {
        accountId = doc["accountId"].toString();
        token = doc["authorizationToken"].toString();
        apiUrl = doc["apiUrl"].toString();
        emit apiConnected();
    } else
        emit authenticationFailed();
}

void B2::connectApi(QString keyId, QString key) {
    nam_->get("https://api.backblazeb2.com/b2api/v2/b2_authorize_account",
              keyId, key, getHandler(&B2::onAuthenticationResponse));
}

void B2::onBucketsReceived(QNetworkReply* rep) {
    auto content = rep->readAll();
    auto doc = QJsonDocument::fromJson(content).object();

    QVector<BucketPointer> buckets;
    for(auto obj : doc["buckets"].toArray()) {
        buckets.push_back(Bucket::fromJson(obj.toObject()));
    }

    emit bucketsReceived(buckets);
}

void B2::getBuckets() {
    auto url = QStringLiteral("%1/b2api/v2/b2_list_buckets").arg(apiUrl);
    nam_->post(url, getJson({"accountId", accountId}), token, getHandler(&B2::onBucketsReceived));
}

void B2::onFileCopied(QNetworkReply* rep, FilePointer f) {
    auto content = rep->readAll();
    auto doc = QJsonDocument::fromJson(content).object();
    auto newFile = File::fromJson(doc);

    if (awaitingCopies.contains(f->id)) {
        awaitingCopies.remove(f->id);
        emit fileCopiedInternal(f, newFile);
    } else
        emit fileCopied(f, newFile);
}

void B2::copyFile(FilePointer f, FileName destination) {
    QString url = QStringLiteral("%1/b2api/v2/b2_copy_file").arg(apiUrl);
    nam_->post(url, getJson({{"sourceFileId", f->id}, {"fileName", destination}}), token,
               [this, f](QNetworkReply* rep) {
                   onFileCopied(rep, f);
               });
}

void B2::onFileDeleted(QNetworkReply* rep, FilePointer f, DeleteMode) {
    auto content = rep->readAll();
    auto doc = QJsonDocument::fromJson(content).object();

    Q_ASSERT(doc["fileId"].toString() == f->id);

    if (awaitingDeletions.contains(f->id)) {
        awaitingDeletions.remove(f->id);
        emit fileDeletedInternal(f);
    } else
        emit fileDeleted(f);
}

void B2::deleteFile(FilePointer f, DeleteMode mode) {
    Q_ASSERT_X(mode == DeleteMode::DeleteLatestVersion, "B2::deleteFile", "Only DeleteMode::DeleteLatestVersion is supported");

    QString url = QStringLiteral("%1/b2api/v2/b2_delete_file_version").arg(apiUrl);
    nam_->post(url, getJson({{"fileId", f->id}, {"fileName", f->fileName}}), token,
               [&, f](QNetworkReply* reply) {
                    onFileDeleted(reply, f, mode);
               });
}

void B2::onFileCopiedInternal(FilePointer from, FilePointer to) {
    qDebug() << from->fileName << "has been copied to" << to->fileName << "requesting delete";
    awaitingMoves[from->id] = { from, to };
    awaitingDeletions.insert(from->id);
    deleteFile(from);
}

void B2::onFileDeletedInternal(FilePointer f) {
    auto pair = awaitingMoves.take(f->id);
    emit fileRenamed(pair.first, pair.second);
}

void B2::renameFile(FilePointer f, FileName newName) {
    awaitingCopies.insert(f->id);
    copyFile(f, newName);
}

void B2::getFiles(BucketPointer b, QString prefix) {
    auto lister = new FileLister(this, b->id, std::move(prefix));
    connect(lister, &FileLister::filesReceived, [this](QVector<FilePointer> files) {
        emit filesReceived(std::move(files));
    });
    lister->get();
}

void FileLister::get() {
    auto url = QStringLiteral("%1/b2api/v2/b2_list_file_names").arg(b2->apiUrl);
    b2->nam_->post(url, B2::getJson({
                                      { "bucketId", bucketId},
                                      { "prefix", prefix},
                                      { "startFileName", nextFileName},
                                      { "maxFileCount", 1000 }
                                    }),
               b2->token,
               [this](QNetworkReply* rep) {
                   onPartialFileListReceived(rep);
               });
}

void FileLister::onPartialFileListReceived(QNetworkReply *rep)  {
    auto response = QJsonDocument::fromJson(rep->readAll()).object();
    nextFileName = response["nextFileName"].toString();

    for(const auto& file : response["files"].toArray()) {
        files.push_back(File::fromJson(file.toObject()));
    }

    if (!nextFileName.isEmpty() && !nextFileName.isNull()) {
        get();
    } else {
        emit filesReceived(files);
    }
}
























