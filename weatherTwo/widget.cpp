#include "widget.h"
#include "ui_widget.h"




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->listView->hide();
    exitButtonActivate();
    setStyleForWidget();
    setStyleForcityLineEdit();
    createCityList();
    setQMapDayWeek();
    defaultCityName();
    todayDateTime();
    defaultReq();
    updateCurrentTime();
    setcityDateLabel();
    setQTimer();

    QObject::connect(ui->SendPushButton,&QPushButton::clicked, this, &Widget::SendPushButtonClicked);
    QObject::connect(ui->cityLineEdit,&QLineEdit::returnPressed, this, &Widget::SendPushButtonClicked);
    QObject::connect(ui->MinusDaypushButton,&QPushButton::clicked, this, &Widget::dateChanged);
    QObject::connect(ui->PlusDaypushButton,&QPushButton::clicked, this, &Widget::dateChanged);

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
    //QListView * lstview = new QListView();
    ui->listView->setModel(proxymodel);
    //QCompleter * completer = new QCompleter(model);
    //QStyledItemDelegate * itemDelegate = new QStyledItemDelegate();
    //completer->setCaseSensitivity(Qt::CaseInsensitive);
    //ui->cityLineEdit->setCompleter(completer);
    QObject::connect(ui->cityLineEdit,&QLineEdit::textEdited, this,&Widget::cityLineEditChanged);
    QObject::connect(ui->cityLineEdit,&QLineEdit::editingFinished, this,&Widget::cityLineEditEditingFinished);
}
void Widget::cityLineEditChanged(QString text)
{
    proxymodel->setFilterWildcard(text);
    ui->listView->show();
    qDebug() << "text = " << text;
}

void Widget::cityLineEditEditingFinished()
{
    ui->listView->hide();
}
void Widget::exitButtonActivate()
{
    QAbstractButton::connect(ui->ExitPushButton, &QPushButton::clicked, [=](){
        QApplication::quit();

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
    timer->start(1000);
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
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
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

    widgetsHide();
    QObject * pushed = QObject::sender();
    if (pushed == ui->PlusDaypushButton)
    {
        date = date.addDays(1);
    }
    else if (pushed == ui->MinusDaypushButton)
    {
        date = date.addDays(-1);
    }
    setcityDateLabel();
    changeDayReq();
}

void Widget::requestReadyToReadDedault(QJsonObject & obj)
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
    QString currentWindSpeed = QString::number(static_cast<int>(currentWindSpeedJsonValue.toDouble()));
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
        int tempInt = (int) tempDouble;
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

}

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
    QString currentWindSpeed = QString::number(static_cast<int>(currentWindSpeedJsonValue.toDouble()));
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
        int tempInt = (int) tempDouble;
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
}



void Widget::defaultCityName()
{
    widgetsHide();
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
        qDebug() << "default index = " << index;
        city = getCityListItem->wordList[index].at(1);
        if (city == "")
        {
            city = getCityListItem->wordList[index].at(0);
        }
        setCityName(city);
        qDebug() << "default city = " << city;
        utc = getCityListItem->wordList[index].at(2).toInt();
        latitude = getCityListItem->wordList[index].at(3);
        longtitude = getCityListItem->wordList[index].at(4);
        longtitude.chop(2);
        qDebug() << "deafult utc = " << utc;
        qDebug() << "deafult latitude = " << latitude;
        qDebug() << "deafult longtitude = " << longtitude;

        //        qDebug() << "date = " << date;
        //        qDebug() << "date start = " << date.toString("yyyy-MM-dd");
        //        QDateTime newdate = date.addDays(6);
        //        qDebug() << "finish start = " << newdate.toString("yyyy-MM-dd");

    }
}


void Widget::SendPushButtonClicked()
{

    widgetsHide();
    cityFull =  ui->cityLineEdit->text();
    qDebug() << "cityFull = " << cityFull ;
    qDebug() << "cityFull length = " << cityFull.length() ;
    if (cityFull.isEmpty() || cityFull == " ")
    {
        ui->cityLineEdit->clear();
        ui->cityLineEdit->setPlaceholderText("Пожалуйста, введите название города и попробуйте еще раз...");
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
        latitude = getCityListItem->wordList[index].at(3);
        longtitude = getCityListItem->wordList[index].at(4);
        longtitude.chop(2);
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
    //    qDebug() << "url = " << url;
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
    QObject::connect(req, &SendRequest::finishJsonObjectCreate, this, &Widget::requestReadyToReadChangeDay );
    req->tryRequest(url);
}


void Widget::setCurrentParametrs(QStringList parametrs)
{
    // List of currentParametrs << currentTemp << currentHumidity << currentWeatherCode << currentWindSpeed << sunriseQList.at(0)<< sunsetQList.at(0);
    ui->currentTempLabel->setText(parametrs.at(0)+" °C");
    ui->currentHumidityValueLabel->setText(parametrs.at(1)+"%");
    ui->currentSunriseTimeValueLabel->setText(parametrs.at(4));
    ui->currentSunsetTimeValueLabel->setText(parametrs.at(5));
    ui->currentWeatherSpeedValueLabel->setText(parametrs.at(3)+" км/ч");
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
    ui->todayForecastWindSpeed1->setText(QString::number(windSpeed.at(0))+" км/ч");
    ui->todayForecastWindSpeed2->setText(QString::number(windSpeed.at(1))+" км/ч");
    ui->todayForecastWindSpeed3->setText(QString::number(windSpeed.at(2))+" км/ч");
    ui->todayForecastWindSpeed4->setText(QString::number(windSpeed.at(3))+" км/ч");
    ui->todayForecastWindSpeed5->setText(QString::number(windSpeed.at(4))+" км/ч");
    ui->todayForecastWindSpeed6->setText(QString::number(windSpeed.at(5))+" км/ч");
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
//    ui->widgetCity->hide();
//    ui->widgetCurrent->hide();
//    ui->widgetTodayForecast->hide();
//    ui->forecastTempWidget->hide();
    QList <QLabel *> tempLabelList = this->findChildren<QLabel *>();
    for (int i = 0; i< tempLabelList.size();i++)
    {
        tempLabelList.at(i)->hide();
    }
    ui->MinusDaypushButton->hide();
    ui->PlusDaypushButton->hide();

    //qDebug() << "tempLabel = " << tempLabel->text();
}

void Widget::widgetsShow()
{
//    ui->widgetCity->show();
//    ui->widgetCurrent->show();
//    ui->widgetTodayForecast->show();
//    ui->forecastTempWidget->show();
    QList <QLabel *> tempLabelList = this->findChildren<QLabel *>();
    for (int i = 0; i< tempLabelList.size();i++)
    {
        tempLabelList.at(i)->show();
    }
    ui->MinusDaypushButton->show();
    ui->PlusDaypushButton->show();
}



