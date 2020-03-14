#include "b2.h"

#include "networkaccessmanager.h"

#include <QJsonDocument>
#include <QJsonObject>

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

QString getJson(QJsonObject obj) {
    return QJsonDocument(obj).toJson();
}

QString getJson(QPair<QString, QString> s) {
    return getJson(QJsonObject({{s.first, s.second}}));
}

void B2::getBuckets() {
    auto url = QStringLiteral("%1/b2api/v2/b2_list_buckets").arg(apiUrl);
    nam_->post(url, getJson({"accountId", accountId}), token, getHandler(&B2::onBucketsReceived));
}

static size_t counter = 0;

QHash<size_t, QVector<FilePointer>> fileResults;

void B2::onPartialFileListReceived(BucketId bucketId, size_t c, QNetworkReply* rep) {
    auto response = QJsonDocument::fromJson(rep->readAll()).object();
    auto nextFileName = response["nextFileName"].toString();

    for(const auto& file : response["files"].toArray()) {
        fileResults[c].push_back(File::fromJson(file.toObject()));
    }

    if (!nextFileName.isEmpty() && !nextFileName.isNull()) {
        getFiles(bucketId, c, nextFileName);
    } else {
        emit filesReceived(fileResults[c]);
        fileResults.remove(c);
    }
}

void B2::getFiles(BucketId bucketId, size_t requestId, QString nextFileName) {
    auto url = QStringLiteral("%1/b2api/v2/b2_list_file_names").arg(apiUrl);
    nam_->post(url, getJson({
                                 { "bucketId", bucketId},
                                 { "prefix", ""},
                                 { "startFileName", nextFileName},
                                 { "maxFileCount", 1000 }
                             }),
               token,
               [this, bucketId, requestId](QNetworkReply* rep) {
                   onPartialFileListReceived(bucketId, requestId, rep);
               });
}

void B2::getFiles(BucketPointer b, QString prefix) {
    auto url = QStringLiteral("%1/b2api/v2/b2_list_file_names").arg(apiUrl);
    nam_->post(url, getJson({
                                 { "bucketId", b->id},
                                 { "prefix", prefix},
                                 { "startFileName", ""},
                                 { "maxFileCount", 1000 }
                             }),
               token,
               [this, b](QNetworkReply* rep) {
                   onPartialFileListReceived(b->id, counter++, rep);
               });
}


































