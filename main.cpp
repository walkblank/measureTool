#include "mainwindow.h"
#include "mainstartup.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);

    QFont font = a.font();
    font.setPointSize(7);
    a.setFont(font);
    MainStartUp w;
    w.show();
    return a.exec();
}
