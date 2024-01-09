#include "stylehelper.h"


QString StyleHelper::getFontColor()
{
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        qDebug() << "Подключен файл стилей: " << file.fileName();
        QString styleSheet = QLatin1String(file.readAll());
        //    QString str = "QWidget#Widget "
        //                  "{"
        //                  "background-color: rgb(146, 146, 146);"
        //                  "}";
        file.close();
        return styleSheet;
    }
    else
    {
        qDebug() << "Не найден файл стилей: " << file.fileName();
        return "";
    }
}

