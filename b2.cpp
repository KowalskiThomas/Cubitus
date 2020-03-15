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
    qInfo() << "Started B2 API";
}

QString getJson(QJsonObject obj) {
    return QJsonDocument(obj).toJson();
}

QString getJson(QPair<QString, QString> s) {
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

void B2::onFileCopied(QNetworkReply* rep) {
    auto content = rep->readAll();
    auto doc = QJsonDocument::fromJson(content).object();
    auto newFile = File::fromJson(doc);

    emit fileCopied(newFile);
}

void B2::copyFile(FilePointer f, FileName destination) {
    QString url = QStringLiteral("%1/b2api/v2/b2_copy_file").arg(apiUrl);
    nam_->post(url, getJson({{"sourceFileId", f->id}, {"fileName", destination}}), token, getHandler(&B2::onFileCopied));
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
    b2->nam_->post(url, getJson({
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
























