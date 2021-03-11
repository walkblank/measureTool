#include "mainwindow.h"
#include "mainstartup.h"

#include <QApplication>
#include <QDateTime>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    if(QDateTime::currentDateTime() > QDateTime::fromString("20210511", "yyyyMMdd"))
    {
        qDebug()<<"invalid version";
        return 0;
    }

    QApplication a(argc, argv);
    QFont font = a.font();
    font.setPointSize(7);
    a.setFont(font);
    MainStartUp w;
    w.show();
    return a.exec();
}
