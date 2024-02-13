#include "widget.h"
#include <QApplication>
 #include <QEvent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Погода"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);
    //QWidget background(nullptr, Qt::FramelessWindowHint);
    //background.setAttribute(Qt::WA_TranslucentBackground);
    Widget w(nullptr, Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_NoSystemBackground);
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.setAttribute(Qt::WA_PaintOnScreen);
    w.show();
    return a.exec();
}
