#ifndef GETCITYLIST_H
#define GETCITYLIST_H

#include <QObject>
#include <QFile>
#include <QDebug>


class GetCityList : public QObject
{
    Q_OBJECT
public:
    explicit GetCityList(QObject *parent = nullptr);
    void GetCityListFunction();
    QList<QStringList> wordList;

signals:

};

#endif // GETCITYLIST_H
