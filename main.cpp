#include "mainWindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QRect screenrect = a.primaryScreen()->geometry();
    w.move(screenrect.left(), screenrect.top());
    w.show();

    return a.exec();
}
