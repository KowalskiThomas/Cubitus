#include "networkaccessmanager.h"

NetworkAccessManager::NetworkAccessManager()
{
    networkAccessManager_ = new QNetworkAccessManager();
}

NetworkAccessManager* NetworkAccessManager::get() {
    if (instance_ == nullptr)
        instance_ = new NetworkAccessManager();

    Q_ASSERT(instance_);
    return instance_;
}

void NetworkAccessManager::basicAuthentication(QNetworkRequest* req, Username username, Password password) {
    if (!username.isNull() && !password.isNull()) {
        QString concat = QStringLiteral("%1:%2").arg(username).arg(password);
        QString encrypted = concat.toUtf8().toBase64();
        QString authorisationHeaderValue = QStringLiteral("Basic %1").arg(encrypted);
        req->setRawHeader("Authorization", authorisationHeaderValue.toUtf8());
    }
}

void NetworkAccessManager::errorTrap(QNetworkReply* reply, QNetworkReply::NetworkError err) {
    qInfo() << "Error in" << reply << err;
}

void NetworkAccessManager::get(QNetworkRequest* req, ReplyHandler::handler_t handler) {
    auto reply = networkAccessManager_->get(*req);
    auto replyHandler = new ReplyHandler(reply, handler);
    QObject::connect(reply, &QNetworkReply::finished, replyHandler, &ReplyHandler::handle);

    /*
    QObject::connect(reply,
                     QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                     std::bind(&NetworkAccessManager::errorTrap, this, reply, std::placeholders::_2));
    */
}

void NetworkAccessManager::get(QString url, QString username, QString password, ReplyHandler::handler_t handler) {
    auto req = QNetworkRequest(QUrl(url));
    basicAuthentication(&req, username, password);
    get(&req, std::move(handler));
}

void NetworkAccessManager::get(QString url, Token token, ReplyHandler::handler_t handler) {
    auto req = QNetworkRequest(QUrl(url));
    req.setRawHeader("Authorization", token.toUtf8());
    get(&req, std::move(handler));
}

void NetworkAccessManager::post(QNetworkRequest *req, QString data, ReplyHandler::handler_t handler) {
    auto reply = networkAccessManager_->post(*req, data.toUtf8());
    auto replyHandler = new ReplyHandler(reply, handler);
    QObject::connect(reply, &QNetworkReply::finished, replyHandler, &ReplyHandler::handle);
    QObject::connect(reply,
                     QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                     [=](QNetworkReply::NetworkError err) {
                        qDebug() << err;
                     });
                     // std::bind(&NetworkAccessManager::errorTrap, this, reply, std::placeholders::_3));
}

void NetworkAccessManager::post(QString url, QString data, QString token, ReplyHandler::handler_t handler) {
    auto req = QNetworkRequest(QUrl(url));
    req.setRawHeader("Authorization", token.toUtf8());
    req.setRawHeader("Content-Type", "text/json");
    post(&req, std::move(data), std::move(handler));
}

