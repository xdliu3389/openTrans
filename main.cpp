#include "mainwindow.h"
#include <QApplication>
#include <QString>

QString filePath;
MainWindow *w;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new MainWindow();
    w->show();

    return a.exec();
}
