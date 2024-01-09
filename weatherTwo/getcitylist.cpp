#include "getcitylist.h"

#include <QFileInfo>

GetCityList::GetCityList(QObject *parent)
    : QObject{parent}
{
    GetCityListFunction();
}

void GetCityList::GetCityListFunction()
{
    QString val;
    QFile file;
    file.setFileName(":/resources/city.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Подключен файл с городами: " << file.fileName();

        QStringList tempList;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            //qDebug() << (line.split(','));
            for (int i = 0; i<5;i++)
            {
                tempList.append(line.split(';').at(i));
            }
            wordList.append(tempList);
            //qDebug() << "tempList = " << tempList;
            tempList.clear();
        }

        file.close();
        for (int i = 0; i<wordList.size();i++ )
        {
            QStringList tempList = wordList[i];
            //qDebug() << tempList.at(1);
        }


    }
    else
    {
        qDebug() << "Не удалось подключить файл:" << file.fileName();
    }
}
