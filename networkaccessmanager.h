#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ReplyHandler : public QObject {
    Q_OBJECT

public:
    using handler_t = std::function<void(QNetworkReply*)>;

private:
    QNetworkReply* reply;
    handler_t handler;

    static inline std::atomic<size_t> handlerCount = 0ull;

public:
    ReplyHandler(QNetworkReply* reply, handler_t handler)
        : reply(reply), handler(handler)
    {
        handlerCount++;
    }

    void handle() {
        handler(reply);
        deleteLater();
    }

    ~ReplyHandler() {
        handlerCount--;
        if (handlerCount > 15) {
            qWarning() << "Possible memory leak detected in ReplyHandler";
        }
    }
};

using Username = QString;
using Password = QString;
using Token = QString;

class NetworkAccessManager
{
    QNetworkAccessManager* networkAccessManager_ = nullptr;
    static inline NetworkAccessManager* instance_ = nullptr;

    void basicAuthentication(QNetworkRequest* req, Username username, Password password);

    NetworkAccessManager();

    void errorTrap(QNetworkReply* reply, QNetworkReply::NetworkError);

public:
    static NetworkAccessManager* get();

    void post(QNetworkRequest* req, QString data, ReplyHandler::handler_t handler);
    // void post(QString url, QString data, Username username, Password password, ReplyHandler::handler_t handler);
    void post(QString url, QString data, Token token, ReplyHandler::handler_t handler);

    void get(QNetworkRequest* req, ReplyHandler::handler_t handler);
    void get(QString url, Username username, Password password, ReplyHandler::handler_t handler);
    void get(QString url, Token token, ReplyHandler::handler_t handler);
};

