#include <QApplication>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

   return app.exec();
}
