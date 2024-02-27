#include "widget.h"
#include "ui_widget.h"

#include <QCursor>
#include <QMovie>




Widget::Widget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , ui(new Ui::Widget)
    ,trayMessageShown(false)
    ,internet(false)
{
    ui->setupUi(this);
    hideMessage();

    widgetsHide();
    loadingShow();
    exitButtonActivate();
    setStyleForWidget();
    setStyleForcityLineEdit();
    createCityList();
    setQMapDayWeek();
    defaultCityName();
    //todayDateTime();
    updateCurrentTime();
    timerInternet = new QTimer(this);
    connect(timerInternet, &QTimer::timeout, this, &Widget::checkInternetConnection);
    timerInternet->start(1000);
    //checkInternetConnection();
    //setcityDateLabel();
    setQTimer();

    QObject::connect(ui->SendPushButton,&QPushButton::clicked, this, &Widget::SendPushButtonClicked);
    QObject::connect(ui->cityLineEdit,&QLineEdit::returnPressed, this, &Widget::SendPushButtonClicked);
    QObject::connect(ui->MinusDaypushButton,&QPushButton::clicked, this, &Widget::dateChanged);
    QObject::connect(ui->PlusDaypushButton,&QPushButton::clicked, this, &Widget::dateChanged);
    QObject::connect(listView,&QListView::clicked, this,&Widget::listViewClicked);

    createActions();
    createTrayIcon();
    setIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Widget::iconActivated);
    trayIcon->show();
    QTimer *timerUpdateEveryHour = new QTimer(this);
    connect(timerUpdateEveryHour, &QTimer::timeout, this, &Widget::defaultReq);
    timerUpdateEveryHour->start(3600000);

    //defaultReq();
    //QObject::connect(date,&QDateTime::dateChanged, this, &Widget::todayDateTime);

}
Widget::~Widget()
{
    delete ui;
}

void Widget::setStyleForWidget()
{
    this->setStyleSheet(StyleHelper::getFontColor());
}

void Widget::setStyleForcityLineEdit()
{
    //add icon to search line
    ui->cityLineEdit->setClearButtonEnabled(true);
    ui->cityLineEdit->addAction(QIcon(":/search.png"), QLineEdit::LeadingPosition);
    ui->cityLineEdit->setPlaceholderText("Начаните вводить название города...");
}

void Widget::setQMapDayWeek()
{
    dayOfTheWeekMap["Monday"] = "Понедельник";
    dayOfTheWeekMap["Tuesday"] = "Вторник";
    dayOfTheWeekMap["Wednesday"] = "Среда";
    dayOfTheWeekMap["Thursday"] = "Четверг";
    dayOfTheWeekMap["Friday"] = "Пятница";
    dayOfTheWeekMap["Saturday"] = "Суббота";
    dayOfTheWeekMap["Sunday"] = "Воскресенье";
    dayOfMonth["1"] = "Января";
    dayOfMonth["2"] = "Февраля";
    dayOfMonth["3"] = "Марта";
    dayOfMonth["4"] = "Апреля";
    dayOfMonth["5"] = "Мая";
    dayOfMonth["6"] = "Июня";
    dayOfMonth["7"] = "Июля";
    dayOfMonth["8"] = "Августа";
    dayOfMonth["9"] = "Сентября";
    dayOfMonth["10"] = "Октября";
    dayOfMonth["11"] = "Ноября";
    dayOfMonth["12"] = "Декабря";
}

void Widget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Widget::createCityList()
{
    getCityListItem = new GetCityList();
    QStringList citylist;
    for (int i =0;i<getCityListItem->wordList.size(); i++)
    {
        citylist.append(getCityListItem->wordList[i].at(0));
    }
    QStringListModel * model = new QStringListModel();
    QStringListModel * modelNew = new QStringListModel();
    model->setStringList(citylist);
    modelNew->setStringList(citylist);
    proxymodel = new QSortFilterProxyModel();
    proxymodel->setSourceModel(modelNew);
    proxymodel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    listView = new QListView(this);
    listView->hide();
    listView->setModel(proxymodel);
    this->setMouseTracking(true);
    //ui->listView->setModel(proxymodel);
    //QCompleter * completer = new QCompleter(model);
    //QStyledItemDelegate * itemDelegate = new QStyledItemDelegate();
    //completer->setCaseSensitivity(Qt::CaseInsensitive);
    //ui->cityLineEdit->setCompleter(completer);
    QObject::connect(ui->cityLineEdit,&QLineEdit::textEdited, this,&Widget::cityLineEditChanged);
    QObject::connect(ui->cityLineEdit,&QLineEdit::editingFinished, this,&Widget::cityLineEditEditingFinished);
    //QObject::connect(ui->cityLineEdit,&QLineEdit::clearButtonClicked, this,&Widget::cityLineEditEditingFinished);
}
void Widget::cityLineEditChanged(QString text)
{
    QPoint cityLineEditPos = ui->cityLineEdit->mapTo(ui->cityLineEdit->window() , ui->cityLineEdit->pos());

    //qDebug() << "cityLineEditPos1 = " << cityLineEditPos ;
    //QPoint cityLineEditPos = ui->cityLineEdit->mapToParent(ui->cityLineEdit->rect().topLeft());;
    QPoint temp(0, ui->cityLineEdit->height()-10);
    cityLineEditPos += temp;
    //qDebug() << "cityLineEditPos = " << cityLineEditPos;
    proxymodel->setFilterWildcard(text);
    listView->resize(ui->cityLineEdit->width(),60);
    listView->move(cityLineEditPos);
    listView->setStyleSheet("background-color: #444;max-height:100;border-bottom-right-radius: 10;border-bottom-left-radius: 10; color:#fff");
    listView->show();
    qDebug() << "text = " << text;
}
void Widget::listViewClicked(const QModelIndex &index)
{
    QString tempcity = index.data(Qt::DisplayRole).toString();
    //qDebug() << "tempcity = " << tempcity;
    ui->cityLineEdit->setText(tempcity);

}


void Widget::cityLineEditEditingFinished()
{
    listView->hide();
}
void Widget::exitButtonActivate()
{
    QAbstractButton::connect(ui->ExitPushButton, &QPushButton::clicked, [=](){
        //QApplication::quit();
        hide();
        showTrayMessage();
    });

}

void Widget::todayDateTime()
{
    date = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone(utc*3600));
}

void Widget::setQTimer()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::updateCurrentTime);
    timer->start(500);
}

QString Widget::returnDayOfTheWeek(QDateTime day)
{
    QString dayStr = day.toString("dddd");
    dayStr = day.toString("dddd");
    return dayOfTheWeekMap[dayStr];

}

QString Widget::returnMonth(QDateTime month)
{
    QString monthStr = month.toString("M");
    return dayOfMonth[monthStr];
}

void Widget::setcityDateLabel()
{
    QString dateLabelStr = returnDayOfTheWeek(date) + ", " + date.toString("d") + " " + returnMonth(date);
    QString forecast1LabelText = returnDayOfTheWeek(date.addDays(1)) + ", " + date.addDays(1).toString("d") + " " + returnMonth(date.addDays(1));
    QString forecast2LabelText = returnDayOfTheWeek(date.addDays(2)) + ", " + date.addDays(2).toString("d") + " " + returnMonth(date.addDays(2));
    QString forecast3LabelText = returnDayOfTheWeek(date.addDays(3)) + ", " + date.addDays(3).toString("d") + " " + returnMonth(date.addDays(3));
    QString forecast4LabelText = returnDayOfTheWeek(date.addDays(4)) + ", " + date.addDays(4).toString("d") + " " + returnMonth(date.addDays(4));
    QString forecast5LabelText = returnDayOfTheWeek(date.addDays(5)) + ", " + date.addDays(5).toString("d") + " " + returnMonth(date.addDays(5));
    ui->cityDateLabel->setText(dateLabelStr);
    ui->forecast1Label->setText(forecast1LabelText);
    ui->forecast2Label->setText(forecast2LabelText);
    ui->forecast3Label->setText(forecast3LabelText);
    ui->forecast4Label->setText(forecast4LabelText);
    ui->forecast5Label->setText(forecast5LabelText);

}

void Widget::setCityName(QString cityName)
{
    ui->cityNameLabel->setText(cityName);
}



void Widget::mousePressEvent(QMouseEvent *event)
{
    mpos = event->pos();
    if (ui->cityLineEdit->geometry().contains(mpos))
    {
        listView->hide();
    }
        qDebug() << mpos;

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;
        this->move(newpos);
    }
//        qDebug() << "x: " << event->pos();
//        qDebug() << "y: " << event->pos().y();




}

QString Widget::formURL(QString lat, QString longt, QString startDate, QString finishDate)
{
    QString url = "https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + longt + "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m,wind_direction_10m&hourly=temperature_2m,relative_humidity_2m,weather_code,wind_speed_180m,wind_direction_180m&daily=weather_code,temperature_2m_max,sunrise,sunset&timezone=auto&start_date=" + startDate + "&end_date=" + finishDate;
    return url;
}



void Widget::updateCurrentTime()
{
    QString current = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone(utc*3600)).toString("hh:mm");
    ui->cityTimeLabel->setText(current);
}

void Widget::dateChanged() // if +1 day plusminus = 1; -1 day plusminus = 0
{
    timerInternet->setInterval(1000);
    widgetsHide();
    loadingShow();
    QDateTime oldDate = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone(utc*3600));
    QDateTime tempDate;
    QString tooltipString;
    QObject * pushed = QObject::sender();
    if (pushed == ui->PlusDaypushButton)
    {
        tempDate = date.addDays(1);
        tooltipString = "Достигнут максимальный прогнозный день";
    }
    else if (pushed == ui->MinusDaypushButton)
    {
        tempDate = date.addDays(-1);
        tooltipString = "Достигнут минимальный прогнозный день";
    }
    qint64 days = oldDate.daysTo(tempDate);
    if (days>-10 && days<10)
    {
        date = tempDate;
        setcityDateLabel();
        changeDayReq();

        qDebug() << "dateChanged() days = " << days;
    }
    else
    {
        loadingHide();
        widgetsShow();
        QPoint cityDayNothingDoLabelPos = ui->cityDayNothingDoLabel->mapTo(ui->cityDayNothingDoLabel->window() , QPoint(0,ui->cityDayNothingDoLabel->height()+10));
        customToolTip(cityDayNothingDoLabelPos, tooltipString);
        //qDebug() << "dateChanged() days = " << days;
    }

}

void Widget::customToolTip(QPoint pos, QString text)
{
    maxDays = new QLabel(this);
    maxDays->setText(text);
    maxDays->setStyleSheet("background-color:#ffffff; color:#444444; padding:5; border-radius:5; font-size:10px;");
    maxDays->move(pos);
    maxDays->show();
    QTimer::singleShot(3000, maxDays, &QLabel::deleteLater);
}
//For Tray
void Widget::setIcon()
{
    QIcon icon = QIcon(":/resources/images/icon.png");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Погода");
    trayMessage = "Приложение Погода скрыто в панель задач";
    setWindowIcon(icon);

}

void Widget::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    //trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    //trayIconMenu->addSection(QIcon(":/resources/images/icon.png"),QString("Открыть"));
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIconMenu->setStyleSheet("color:#000;");


}

void Widget::createActions()
{
    //minimizeAction = new QAction(tr("Mi&nimize"), this);
    //connect(minimizeAction, &QAction::triggered, this, &Widget::hide);
    //maximizeAction = new QAction(tr("Ma&ximize"), this);
    //connect(maximizeAction, &QAction::triggered, this, &Widget::showMaximized);
    restoreAction = new QAction(tr("&Открыть"), this);
    restoreAction->setIcon(QIcon(":/resources/images/icon.png"));
    connect(restoreAction, &QAction::triggered, this, &Widget::showNormal);
    quitAction = new QAction(tr("&Выйти"), this);
    quitAction->setIcon(QIcon(":/resources/images/exit.png"));
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    //minimizeAction->setEnabled(true);
    //maximizeAction->setEnabled(!isMaximized());
    //restoreAction->setEnabled(isMaximized());
    //QWidget::setVisible(true);
}
void Widget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        Widget::showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}

void Widget::closeEvent(QCloseEvent* e) {
    //QMessageBox::information(this, "", "Close event received.");
    //QMainWindow::closeEvent(e);
    showTrayMessage();
}

void Widget::showTrayMessage()
{
    if (!trayMessageShown)
    {
       trayIcon->showMessage("Погода", trayMessage, trayIcon->icon(), 10000);
       trayMessageShown = true;
    }

}

//bool Widget::event(QEvent *ev)
//{
//    if (ev->type() == QEvent::MouseMove)
//    {
//       qDebug() << "x: = " << QCursor::pos().x() << " y = " << QCursor::pos().y();
//    }
//    QObject::event(ev);
//    return true;
//}

void Widget::setTrayFilterEvent()
{

}

bool Widget::checkInternetConnection()
{

    QTcpSocket* sock = new QTcpSocket(this);
    sock->connectToHost("open-meteo.com", 80);
    bool connected = sock->waitForConnected(5000);//ms
    if (!connected)
    {
       if (ui->widgetStatus->isHidden())
       {
           showMessage(QString("Отсутствует интернет соединение..."));
           qDebug() << "Now you are offline!";
           timerInternet->setInterval(3000);
       }
       else
       {
           timerInternet->setInterval(10000);
       }
       qDebug() << "You still have no internet!";
       internet = false;
       sock->abort();
       return false;
    }
    if (!internet)
    {
       internet = true;
       qDebug() << "Now you are online!";
       hideMessage();
       timerInternet->setInterval(1800000);
       defaultReq();
    }
    timerInternet->setInterval(1800000);
    qDebug() << "Wow you are still have internet!";
    sock->close();
    return true;
}

void Widget::showMessage(QString message)
{

    ui->statusLabel->setText(message);
    ui->widgetStatus->show();
    ui->statusLabel->show();
}

void Widget::hideMessage()
{
    ui->statusLabel->show();
    ui->widgetStatus->hide();
}



void Widget::requestReadyToReadDedault(QJsonObject & obj)
{
    if (obj.isEmpty())
    {
       //showMessage(QString("Отсутствует интернет соединение..."));
       //qDebug() << "QJsonObject & obj is empty";
    }
    else
    {
    qDebug() << "request finished. And got in the widget.";
    //qDebug() << "obj = " << obj;
    QJsonObject obj1 = obj["current"].toObject();
    QJsonValue currentTempJsonValue = obj1["temperature_2m"];
    QJsonValue currentHumidityJsonValue = obj1["relative_humidity_2m"];
    QJsonValue currentWeatherCodeJsonValue = obj1["weather_code"];
    QJsonValue currentWindDirectionJsonValue = obj1["wind_direction_10m"];
    QJsonValue currentWindSpeedJsonValue = obj1["wind_speed_10m"];
    QString currentTemp = QString::number(static_cast<int>(currentTempJsonValue.toDouble()));
    QString currentHumidity = QString::number(currentHumidityJsonValue.toInt());
    QString currentWeatherCode = QString::number(currentWeatherCodeJsonValue.toInt());
    QString currentWindDirection = QString::number(currentWindDirectionJsonValue.toInt());
    QString currentWindSpeed = QString::number(static_cast<int>(currentWindSpeedJsonValue.toDouble()/3.6));
    QStringList currentParametrs;


    //    qDebug() << "currentTemp = " << currentTemp;
    //    qDebug() << "currentHumidity = " << currentHumidity;
    //    qDebug() << "currentWeatherCode = " << currentWeatherCode;
    //    qDebug() << "currentWindDirection = " << currentWindDirection;
    //    qDebug() << "currentWindSpeed = " << currentWindSpeed;

    //sunrise
    QJsonObject dailyObj = obj["daily"].toObject();
    QJsonArray sunriseArray = dailyObj["sunrise"].toArray();
    QList<QString> sunriseQList;
    for (int i = 0;i< sunriseArray.size();i++)
    {
        QString tempString = sunriseArray[i].toString().remove(0,11);
        sunriseQList.append(tempString);
        //    qDebug() << "sunriseArray["<< i << "] = " << tempString;
    }

    //sunset
    QJsonArray sunsetArray = dailyObj["sunset"].toArray();
    QList<QString> sunsetQList;
    for (int i = 0;i< sunsetArray.size();i++)
    {
        QString tempString = sunsetArray[i].toString().remove(0,11);
        sunsetQList.append(tempString);
        //    qDebug() << "sunsetArray["<< i << "] = " << tempString;
    }
    currentParametrs << currentTemp << currentHumidity << currentWeatherCode << currentWindSpeed << sunriseQList.at(0)<< sunsetQList.at(0);

    //day temperature 24 hours
    QJsonObject hourlyObj = obj["hourly"].toObject();
    QJsonArray tempArray = hourlyObj["temperature_2m"].toArray();
    QList<int> temp24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = tempArray[i].toDouble();
        int tempInt = (int) tempDouble;
        temp24HoursQList.append(tempInt);
        //    qDebug() << "tempArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< temp24HoursQList.size();i++)
    {
        //    qDebug() << "temp24HoursQList["<< i << "] = " << temp24HoursQList[i];

    }
    // weather code 24 hours
    QJsonArray weatherCode24HoursArray = hourlyObj["weather_code"].toArray();
    QList<int> weatherCode24HoursHoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        int tempInt = weatherCode24HoursArray[i].toInt();
        weatherCode24HoursHoursQList.append(tempInt);
        //    qDebug() << "weatherCode24HoursArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< weatherCode24HoursHoursQList.size();i++)
    {
        //    qDebug() << "weatherCode24HoursHoursQList["<< i << "] = " << weatherCode24HoursHoursQList[i];
    }

    //day wind speed
    QJsonArray windSpeedArray = hourlyObj["wind_speed_180m"].toArray();
    QList<int> windSpeed24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = windSpeedArray[i].toDouble();
        int tempInt = (int) tempDouble/3.6;
        windSpeed24HoursQList.append(tempInt);
        //    qDebug() << "windSpeedArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< windSpeed24HoursQList.size();i++)
    {
        //    qDebug() << "windSpeed24HoursQList["<< i << "] = " << windSpeed24HoursQList[i];
    }

    //day wind direction
    QJsonArray windDirectionArray = hourlyObj["wind_direction_180m"].toArray();
    QList<int> windDirection24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = windDirectionArray[i].toDouble();
        int tempInt = (int) tempDouble;
        windDirection24HoursQList.append(tempInt);
        //    qDebug() << "windDirectionArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< windDirection24HoursQList.size();i++)
    {
        //    qDebug() << "windDirection24HoursQList["<< i << "] = " << windDirection24HoursQList[i];
    }

    // 5 day temperature forecast
    QList<int> tempFiveForecastQList;
    for (int i = 38;i< tempArray.size();i += 24)
    {
        double tempDouble = tempArray[i].toDouble();
        int tempInt = (int) tempDouble;
        tempFiveForecastQList.append(tempInt);
        //    qDebug() << "tempArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< tempFiveForecastQList.size();i++)
    {
        //    qDebug() << "tempFiveForecastQList["<< i << "] = " << tempFiveForecastQList[i];

    }

    // weather code 5 day forecast
    QJsonArray weatherCodeFiveForecastArray = dailyObj["weather_code"].toArray();
    QList<int> weatherCodeFiveForecastQList;
    for (int i = 0;i< weatherCodeFiveForecastArray.size();i++)
    {
        int tempInt = weatherCodeFiveForecastArray[i].toInt();
        weatherCodeFiveForecastQList.append(tempInt);
        //    qDebug() << "weatherCodeFiveForecastArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< weatherCodeFiveForecastQList.size();i++)
    {
        //    qDebug() << "weatherCodeFiveForecastQList["<< i << "] = " << weatherCodeFiveForecastQList[i];

    }
    setCurrentParametrs(currentParametrs);
    setFiveDayForecastParametrs(weatherCodeFiveForecastQList,tempFiveForecastQList);
    setTodayForecastParametrs(weatherCode24HoursHoursQList,temp24HoursQList, windSpeed24HoursQList,windDirection24HoursQList);
    widgetsShow();
    loadingHide();
    }
}


// dell
void Widget::requestReadyToReadChangeDay(QJsonObject &obj)
{
    qDebug() << "request finished. And got in the widget.";
    //qDebug() << "obj = " << obj;

    QJsonObject obj1 = obj["current"].toObject();
    QJsonValue currentTempJsonValue = obj1["temperature_2m"];
    QJsonValue currentHumidityJsonValue = obj1["relative_humidity_2m"];
    QJsonValue currentWeatherCodeJsonValue = obj1["weather_code"];
    QJsonValue currentWindDirectionJsonValue = obj1["wind_direction_10m"];
    QJsonValue currentWindSpeedJsonValue = obj1["wind_speed_10m"];
    QString currentTemp = QString::number(static_cast<int>(currentTempJsonValue.toDouble()));
    QString currentHumidity = QString::number(currentHumidityJsonValue.toInt());
    QString currentWeatherCode = QString::number(currentWeatherCodeJsonValue.toInt());
    QString currentWindDirection = QString::number(currentWindDirectionJsonValue.toInt());
    QString currentWindSpeed = QString::number(static_cast<int>(currentWindSpeedJsonValue.toDouble()/3.6));
    QStringList currentParametrs;


    //    qDebug() << "currentTemp = " << currentTemp;
    //    qDebug() << "currentHumidity = " << currentHumidity;
    //    qDebug() << "currentWeatherCode = " << currentWeatherCode;
    //    qDebug() << "currentWindDirection = " << currentWindDirection;
    //    qDebug() << "currentWindSpeed = " << currentWindSpeed;

    //sunrise
    QJsonObject dailyObj = obj["daily"].toObject();
    QJsonArray sunriseArray = dailyObj["sunrise"].toArray();
    QList<QString> sunriseQList;
    for (int i = 0;i< sunriseArray.size();i++)
    {
        QString tempString = sunriseArray[i].toString().remove(0,11);
        sunriseQList.append(tempString);
        //    qDebug() << "sunriseArray["<< i << "] = " << tempString;
    }

    //sunset
    QJsonArray sunsetArray = dailyObj["sunset"].toArray();
    QList<QString> sunsetQList;
    for (int i = 0;i< sunsetArray.size();i++)
    {
        QString tempString = sunsetArray[i].toString().remove(0,11);
        sunsetQList.append(tempString);
        //    qDebug() << "sunsetArray["<< i << "] = " << tempString;
    }
    currentParametrs << currentTemp << currentHumidity << currentWeatherCode << currentWindSpeed << sunriseQList.at(0)<< sunsetQList.at(0);

    //day temperature 24 hours
    QJsonObject hourlyObj = obj["hourly"].toObject();
    QJsonArray tempArray = hourlyObj["temperature_2m"].toArray();
    QList<int> temp24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = tempArray[i].toDouble();
        int tempInt = (int) tempDouble;
        temp24HoursQList.append(tempInt);
        //    qDebug() << "tempArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< temp24HoursQList.size();i++)
    {
        //    qDebug() << "temp24HoursQList["<< i << "] = " << temp24HoursQList[i];

    }
    // weather code 24 hours
    QJsonArray weatherCode24HoursArray = hourlyObj["weather_code"].toArray();
    QList<int> weatherCode24HoursHoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        int tempInt = weatherCode24HoursArray[i].toInt();
        weatherCode24HoursHoursQList.append(tempInt);
        //    qDebug() << "weatherCode24HoursArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< weatherCode24HoursHoursQList.size();i++)
    {
        //    qDebug() << "weatherCode24HoursHoursQList["<< i << "] = " << weatherCode24HoursHoursQList[i];
    }

    //day wind speed
    QJsonArray windSpeedArray = hourlyObj["wind_speed_180m"].toArray();
    QList<int> windSpeed24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = windSpeedArray[i].toDouble();
        int tempInt = (int) tempDouble/3.6;
        windSpeed24HoursQList.append(tempInt);
        //    qDebug() << "windSpeedArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< windSpeed24HoursQList.size();i++)
    {
        //    qDebug() << "windSpeed24HoursQList["<< i << "] = " << windSpeed24HoursQList[i];
    }

    //day wind direction
    QJsonArray windDirectionArray = hourlyObj["wind_direction_180m"].toArray();
    QList<int> windDirection24HoursQList;
    for (int i = 4;i< 25;i = i+4)
    {
        double tempDouble = windDirectionArray[i].toDouble();
        int tempInt = (int) tempDouble;
        windDirection24HoursQList.append(tempInt);
        //    qDebug() << "windDirectionArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< windDirection24HoursQList.size();i++)
    {
        //    qDebug() << "windDirection24HoursQList["<< i << "] = " << windDirection24HoursQList[i];
    }

    // 5 day temperature forecast
    QList<int> tempFiveForecastQList;
    for (int i = 38;i< tempArray.size();i += 24)
    {
        double tempDouble = tempArray[i].toDouble();
        int tempInt = (int) tempDouble;
        tempFiveForecastQList.append(tempInt);
        //    qDebug() << "tempArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< tempFiveForecastQList.size();i++)
    {
        //    qDebug() << "tempFiveForecastQList["<< i << "] = " << tempFiveForecastQList[i];

    }

    // weather code 5 day forecast
    QJsonArray weatherCodeFiveForecastArray = dailyObj["weather_code"].toArray();
    QList<int> weatherCodeFiveForecastQList;
    for (int i = 0;i< weatherCodeFiveForecastArray.size();i++)
    {
        int tempInt = weatherCodeFiveForecastArray[i].toInt();
        weatherCodeFiveForecastQList.append(tempInt);
        //    qDebug() << "weatherCodeFiveForecastArray["<< i << "] = " << tempInt;
    }
    for (int i = 0;i< weatherCodeFiveForecastQList.size();i++)
    {
        //    qDebug() << "weatherCodeFiveForecastQList["<< i << "] = " << weatherCodeFiveForecastQList[i];

    }
    setCurrentParametrs(currentParametrs);
    setFiveDayForecastParametrs(weatherCodeFiveForecastQList,tempFiveForecastQList);
    setTodayForecastParametrs(weatherCode24HoursHoursQList,temp24HoursQList, windSpeed24HoursQList,windDirection24HoursQList);
    widgetsShow();
    loadingHide();
}



void Widget::defaultCityName()
{

    QSettings settings("NewBestCorp", "WeatherApp");
    city = settings.value("city").toString();
    if (city == "" || city == " ")
    {
        city = "Москва";
    }
    qDebug() << "default city = " << city;
    int index = -1;
    for (int i =0;i<getCityListItem->wordList.size(); i++)
    {
        index = getCityListItem->wordList[i].at(1).indexOf(city);
        if (index>-1)
        {
            index = i;
            break;
        }
        index = getCityListItem->wordList[i].at(0).indexOf(city);
        if (index>-1)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        //qDebug() << "default index = " << index;
        city = getCityListItem->wordList[index].at(1);
        if (city == "")
        {
            city = getCityListItem->wordList[index].at(0);
        }
        setCityName(city);
        //qDebug() << "default city = " << city;
        utc = getCityListItem->wordList[index].at(2).toInt();
        latitude = getCityListItem->wordList[index].at(4);
        longtitude = getCityListItem->wordList[index].at(3);
        latitude.chop(2);
        //qDebug() << "deafult utc = " << utc;
        //qDebug() << "deafult latitude = " << latitude;
        //() << "deafult longtitude = " << longtitude;

        //        qDebug() << "date = " << date;
        //        qDebug() << "date start = " << date.toString("yyyy-MM-dd");
        //        QDateTime newdate = date.addDays(6);
        //        qDebug() << "finish start = " << newdate.toString("yyyy-MM-dd");
        todayDateTime();
        setcityDateLabel();
        defaultReq();
    }
}


void Widget::SendPushButtonClicked()
{
    timerInternet->setInterval(1000);
    listView->hide();
    widgetsHide();
    loadingShow();
    cityFull =  ui->cityLineEdit->text();
    qDebug() << "cityFull = " << cityFull ;
    qDebug() << "cityFull length = " << cityFull.length() ;
    if (cityFull.isEmpty() || cityFull == " ")
    {
        ui->cityLineEdit->clear();
        ui->cityLineEdit->setPlaceholderText("Пожалуйста, введите название города и попробуйте еще раз...");
        widgetsShow();
        loadingHide();
        return;
    }
    int index = -1;
    for (int i =0;i<getCityListItem->wordList.size(); i++)
    {
        index = getCityListItem->wordList[i].at(0).indexOf(cityFull );
        if (index>-1)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        qDebug() << "index = " << index;
        city = getCityListItem->wordList[index].at(1);
        QSettings settings("NewBestCorp", "WeatherApp");
        settings.setValue("city", city);
        setCityName(city);
        qDebug() << "city = " << city;
        utc = getCityListItem->wordList[index].at(2).toInt();
        latitude = getCityListItem->wordList[index].at(4);
        longtitude = getCityListItem->wordList[index].at(3);
        latitude.chop(2);
        todayDateTime();
        setcityDateLabel();
        qDebug() << "utc = " << utc;
        qDebug() << latitude;
        qDebug() << longtitude;
        defaultReq();
    }
    else
    {
        ui->cityLineEdit->clear();
        ui->cityLineEdit->setPlaceholderText("Город не найден. Пожалуйста, попробуйте ввести название повторно...");
        widgetsShow();
        loadingHide();
        return;
    }
}


void Widget::defaultReq()
{
    //this->setStyleSheet("QLabel{color:transparent;}");

    dateStart = date.toString("yyyy-MM-dd");
    QDateTime newDate = date.addDays(5);
    dateFinish = newDate.toString("yyyy-MM-dd");
    qDebug() << "date start = " << date.toString("yyyy-MM-dd");
    qDebug() << "finish start = " << newDate.toString("yyyy-MM-dd");
    QString url = formURL(longtitude, latitude, dateStart, dateFinish);
    //qDebug() << "url = " << url;
    req = new SendRequest();
    QObject::connect(req, &SendRequest::finishJsonObjectCreate, this, &Widget::requestReadyToReadDedault );
    req->tryRequest(url);
}

void Widget::changeDayReq()
{

    dateStart = date.toString("yyyy-MM-dd");
    QDateTime newDate = date.addDays(5);
    dateFinish = newDate.toString("yyyy-MM-dd");
    qDebug() << "date start = " << date.toString("yyyy-MM-dd");
    qDebug() << "finish start = " << newDate.toString("yyyy-MM-dd");
    QString url = formURL(longtitude, latitude, dateStart, dateFinish);
    //    qDebug() << "url = " << url;
    req = new SendRequest();
    QObject::connect(req, &SendRequest::finishJsonObjectCreate, this, &Widget::requestReadyToReadDedault );
    req->tryRequest(url);
}


void Widget::setCurrentParametrs(QStringList parametrs)
{
    // List of currentParametrs << currentTemp << currentHumidity << currentWeatherCode << currentWindSpeed << sunriseQList.at(0)<< sunsetQList.at(0);
    ui->currentTempLabel->setText(parametrs.at(0)+" °C");
        ui->currentHumidityValueLabel->setText(parametrs.at(1)+"%");
    ui->currentSunriseTimeValueLabel->setText(parametrs.at(4));
    ui->currentSunsetTimeValueLabel->setText(parametrs.at(5));
    ui->currentWeatherSpeedValueLabel->setText(parametrs.at(3)+" м/с");
    int code = parametrs.at(2).toInt();
    QString res = analyzeWeatherCode(code);
    ui->currentWeatherImg->setPixmap(QPixmap(":/resources/images/header-" + res));
}

void Widget::setFiveDayForecastParametrs(QList<int> weatherCode, QList<int> temp)
{
    ui->forecast1TempLabel->setText(QString::number(temp.at(0))+" °C");
        ui->forecast2TempLabel->setText(QString::number(temp.at(1))+" °C");
        ui->forecast3TempLabel->setText(QString::number(temp.at(2))+" °C");
        ui->forecast4TempLabel->setText(QString::number(temp.at(3))+" °C");
        ui->forecast5TempLabel->setText(QString::number(temp.at(4))+" °C");
        QString res = analyzeWeatherCode(weatherCode.at(0));
    ui->forecast1Img->setPixmap(QPixmap(":/resources/images/right/right_" + res));
    res = analyzeWeatherCode(weatherCode.at(1));
    ui->forecast2Img->setPixmap(QPixmap(":/resources/images/right/right_" + res));
    res = analyzeWeatherCode(weatherCode.at(2));
    ui->forecast3Img->setPixmap(QPixmap(":/resources/images/right/right_" + res));
    res = analyzeWeatherCode(weatherCode.at(3));
    ui->forecast4Img->setPixmap(QPixmap(":/resources/images/right/right_" + res));
    res = analyzeWeatherCode(weatherCode.at(4));
    ui->forecast5Img->setPixmap(QPixmap(":/resources/images/right/right_" + res));
}

void Widget::setTodayForecastParametrs(QList<int> weatherCode, QList<int> temp, QList<int> windSpeed, QList<int> windDirection)
{
    ui->todayForecastTemp1->setText(QString::number(temp.at(0))+" °C");
        ui->todayForecastTemp2->setText(QString::number(temp.at(1))+" °C");
        ui->todayForecastTemp3->setText(QString::number(temp.at(2))+" °C");
        ui->todayForecastTemp4->setText(QString::number(temp.at(3))+" °C");
        ui->todayForecastTemp5->setText(QString::number(temp.at(4))+" °C");
        ui->todayForecastTemp6->setText(QString::number(temp.at(5))+" °C");
        ui->todayForecastWindSpeed1->setText(QString::number(windSpeed.at(0))+" м/с");
    ui->todayForecastWindSpeed2->setText(QString::number(windSpeed.at(1))+" м/с");
    ui->todayForecastWindSpeed3->setText(QString::number(windSpeed.at(2))+" м/с");
    ui->todayForecastWindSpeed4->setText(QString::number(windSpeed.at(3))+" м/с");
    ui->todayForecastWindSpeed5->setText(QString::number(windSpeed.at(4))+" м/с");
    ui->todayForecastWindSpeed6->setText(QString::number(windSpeed.at(5))+" м/с");
    QString res = analyzeWeatherCode(weatherCode.at(0));
    ui->todayForecastImg1->setPixmap(QPixmap(":/resources/images/main/" + res));
    res = analyzeWeatherCode(weatherCode.at(1));
    ui->todayForecastImg2->setPixmap(QPixmap(":/resources/images/main/" + res));
    res = analyzeWeatherCode(weatherCode.at(2));
    ui->todayForecastImg3->setPixmap(QPixmap(":/resources/images/main/" + res));
    res = analyzeWeatherCode(weatherCode.at(3));
    ui->todayForecastImg4->setPixmap(QPixmap(":/resources/images/main/" + res));
    res = analyzeWeatherCode(weatherCode.at(4));
    ui->todayForecastImg5->setPixmap(QPixmap(":/resources/images/main/" + res));
    res = analyzeWeatherCode(weatherCode.at(5));
    ui->todayForecastImg6->setPixmap(QPixmap(":/resources/images/main/" + res));

}

QString Widget::analyzeWeatherCode(int code)
{
    QString res;
    switch(code)
    {
    case 0:
        res = "sunny.png";
        break;
    case 1:
    case 2:
        res = "sunny-cloudy.png";
        break;
    case 3:
    case 45:
    case 48:
    case 51:
    case 53:
    case 55:
        res = "cloudy.png";
        break;
    case 56:
    case 57:
    case 61:
    case 63:
    case 65:
    case 66:
    case 67:
    case 80:
    case 81:
    case 82:
    case 95:
    case 96:
    case 99:
        res = "rainy.png";
        break;
    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86:
        res = "snowy.png";
        break;
    default:
        res = "sunny.png";
        break;
    }
    return res;
}

void Widget::widgetsHide()
{

    QList<QWidget *> widgetList;
    widgetList.append(ui->widgetCity);
    widgetList.append(ui->widgetCurrent);
    widgetList.append(ui->widgetTodayForecast);
    widgetList.append(ui->forecastTempWidget);
    for (int i = 0; i< widgetList.size();i++)
    {
        QList <QLabel *> tempLabelList = widgetList.at(i)->findChildren<QLabel *>();
        QList <QWidget *> widgetTodayForecastList = ui->widgetTodayForecast->findChildren<QWidget *>();
        for (int i = 0; i< tempLabelList.size();i++)
        {
            tempLabelList.at(i)->hide();
            widgetTodayForecastList.at(i)->hide();
        }
    }
    ui->MinusDaypushButton->hide();
    ui->PlusDaypushButton->hide();






    //qDebug() << "tempLabel = " << tempLabel->text();
}

void Widget::widgetsShow()
{

    QList<QWidget *> widgetList;
    widgetList.append(ui->widgetCity);
    widgetList.append(ui->widgetCurrent);
    widgetList.append(ui->widgetTodayForecast);
    widgetList.append(ui->forecastTempWidget);
    for (int i = 0; i< widgetList.size();i++)
    {
        QList <QLabel *> tempLabelList = widgetList.at(i)->findChildren<QLabel *>();
        QList <QWidget *> widgetTodayForecastList = ui->widgetTodayForecast->findChildren<QWidget *>();
        for (int i = 0; i< tempLabelList.size();i++)
        {
            tempLabelList.at(i)->show();
            widgetTodayForecastList.at(i)->show();
        }
    }

}

void Widget::loadingShow()
{
    //loading animation
    int movieSize = 50;
    QPoint pos1;
    mvMovie = new QMovie("://resources/images/loading/Spinner7.gif");
    mvMovie ->setScaledSize(QSize(movieSize,movieSize));
    QList<QWidget *> widgetList;
    widgetList.append(ui->widgetCity);
    widgetList.append(ui->widgetCurrent);
    widgetList.append(ui->widgetTodayForecast);
    widgetList.append(ui->forecastTempWidget);
    for (int i = 0; i<4; i++)
    {
        labelLodaing[i] = new QLabel(widgetList.at(i));
        labelLodaing[i]->resize(movieSize,movieSize);
        //pos1 = widgetList.at(i)->mapTo(widgetList.at(i)->window(), widgetList.at(i)->rect().center());
        pos1 = widgetList.at(i)->rect().center();
        qDebug() << "pos1 = " << pos1;
        //qDebug() << "pos2["<< i << "] = " << pos2[i];
        pos1 = pos1 - QPoint(movieSize/2,movieSize/2);
        labelLodaing[i]->setMovie(mvMovie);
        mvMovie ->start();
        labelLodaing[i]->move(pos1);
        labelLodaing[i]->setAttribute(Qt::WA_NoSystemBackground);
    }
    //loading animation show
    for (int i = 0; i<4; i++)
    {
        labelLodaing[i]->show();
    }

    QEventLoop loop;
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();
}

void Widget::loadingHide()
{
    mvMovie->stop();
    for (int i = 0; i<4; i++)
    {
        labelLodaing[i]->hide();
        labelLodaing[i]->clear();
    }
    ui->MinusDaypushButton->show();
    ui->PlusDaypushButton->show();
}





