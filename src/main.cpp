#include "Bad.h"
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   TrafficView w;
   w.show();

   return a.exec();
}
