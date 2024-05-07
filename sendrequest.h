#ifndef SENDREQUEST_H
#define SENDREQUEST_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

class SendRequest : public QObject
{
    Q_OBJECT
public:
    explicit SendRequest(QObject *parent = nullptr);
    void tryRequest(QString giveUrl);


private:
    QNetworkReply * reply;
    QJsonObject obj;
    QNetworkAccessManager manager;

public slots:
    void finishRequest();


signals:
    void finishJsonObjectCreate(QJsonObject & obj);
};

#endif // SENDREQUEST_H
