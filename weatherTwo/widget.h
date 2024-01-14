#ifndef WIDGET_H
#define WIDGET_H

#include <QPainter>
#include <QStyleOption>
#include <QWidget>
#include <QFile>
#include <stylehelper.h>
#include <QFontDatabase>
#include "getcitylist.h"
#include <QCompleter>
#include <QStringListModel>
#include <sendrequest.h>
#include <QDateTime>
#include <QTimer>
#include <QMouseEvent>
#include <QJsonArray>
#include <QSettings>
#include <QTimeZone>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QListView>
#include <QLabel>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    void setStyleForWidget();
    void setStyleForcityLineEdit();
    QMap<QString, QString> dayOfTheWeekMap;
    QMap<QString, QString> dayOfMonth;
    void setQMapDayWeek();
    QDateTime date;
    void paintEvent(QPaintEvent *);
    GetCityList * getCityListItem;
    void createCityList();
    QString cityFull;
    QString city;
    void exitButtonActivate();
    void todayDateTime();
    void setQTimer();
    QString returnDayOfTheWeek(QDateTime day);
    QString returnMonth(QDateTime month);
    int utc;
    void setcityDateLabel();
    void setCityName(QString cityName);
    void defaultCityName();
    SendRequest * req;
    void defaultReq();
    void changeDayReq();
    QPoint mpos;
    // for window moving
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QString formURL(QString lat,QString longt, QString startDate, QString finishDate);
    QString latitude;
    QString longtitude;
    QString dateStart;
    QString dateFinish;
    QSortFilterProxyModel * proxymodel;
    QListView * listView;
    QLabel * maxDays;
    QLabel * labelLodaing[5];
    QMovie * mvMovie;
    void setCurrentParametrs(QStringList parametrs);
    void setFiveDayForecastParametrs(QList<int> weatherCode, QList<int> temp);
    void setTodayForecastParametrs(QList<int> weatherCode, QList<int> temp, QList<int> windSpeed, QList<int> windDirection);
    QString analyzeWeatherCode(int code);
    void widgetsHide();
    void widgetsShow();
    void customToolTip(QPoint pos, QString text);

public slots:
    void updateCurrentTime();
    void dateChanged();
    void SendPushButtonClicked();
    void requestReadyToReadDedault(QJsonObject & obj);
    void requestReadyToReadChangeDay(QJsonObject & obj);
    void cityLineEditChanged(QString text);
    void cityLineEditEditingFinished();
    void listViewClicked(const QModelIndex &index);
};
#endif // WIDGET_H
