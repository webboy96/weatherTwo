#include "sendrequest.h"



SendRequest::SendRequest(QObject *parent)
    : QObject{parent}
{

}

void SendRequest::tryRequest(QString giveUrl)
{

    QString url = giveUrl;
    //qDebug() << "giveUrl = " << giveUrl;
    reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, this,  &SendRequest::finishRequest);
}

void SendRequest::finishRequest()
{
    if (reply ->error() == QNetworkReply::NoError)
    {
        qDebug() << "No error. Internet connection ok";
        QByteArray data = reply->readAll();
        QJsonDocument jsondocument = QJsonDocument::fromJson(data);
        obj = jsondocument.object();
        emit finishJsonObjectCreate(obj);
    }
    else
    {
            emit finishJsonObjectCreate(obj);
//        QFile file;
//        file.setFileName(":/resources/test.json");
//        if (file.open(QIODevice::ReadOnly))
//        {
//            while (!file.atEnd())
//            {
//                qDebug() << "Нет интернет соединения. Файл " << file.fileName() << " открыт!";
//                QByteArray data = file.readAll();
//                QJsonDocument jsondocument = QJsonDocument::fromJson(data);
//                obj = jsondocument.object();
//                emit finishJsonObjectCreate(obj);
//            }
//        }
    }
}



