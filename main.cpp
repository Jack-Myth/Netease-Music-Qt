#include "mainwindow.h"
#include "searchwindow.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //SearchWindow w;
    w.show();
    //QMessageBox::information(nullptr,"WorkDir", QApplication::applicationDirPath(),QMessageBox::Ok);
    return a.exec();
}
