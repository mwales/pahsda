#include "TrafficView.h"
#include "ui_TrafficView.h"

#include <QMessageBox>
#include <QtDebug>

#include "PortChooser.h"

TrafficView::TrafficView(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::TrafficView),
   theInterface(nullptr)
{
   ui->setupUi(this);

   // Rich text testing stuff
   ui->label->setTextFormat(Qt::RichText);

   QString text = "<style>\n";
   text += "  .changed { color: red; background-color: green }\n";
   text += "</style>";
   text += "what's up <u>00</u> in <b class='changed'>FF!</b>";
   ui->label->setText(text);

   connect(ui->actionAbout, &QAction::triggered,
           this, &TrafficView::showAbout);
   connect(ui->actionOpen_Serial_Port, &QAction::triggered,
           this, &TrafficView::openSerialPort);
}

TrafficView::~TrafficView()
{
   delete ui;

   if (theInterface != nullptr)
   {
      qDebug() << "Closing the io device before we exit";
      theInterface->close();
   }
}

void TrafficView::showAbout()
{
   qDebug() << "Showing about";
   QMessageBox::about(this,
                            "About PAHSDA",
                            "PAHSDA\n"
                            "Protocol Analysis Highlighting Structured Data for Analysis\n"
                            "Version 1.0");
}

void TrafficView::openSerialPort()
{
   if (theInterface != nullptr)
   {
      qDebug() << "Closing the old interface before propmting user to choose new one";
      theInterface->close();

      theInterface->deleteLater();

      theInterface = nullptr;
   }

   PortChooser pc(this);
   qDebug() << "About to show it";
   pc.show();

   qDebug() << "About to exec it";
   pc.exec();

   qDebug() << "Port chooser complete";

   theInterface = pc.getIoDevice();

   if (theInterface == nullptr)
   {
      qDebug() << "User didn't open anything...";
      return;
   }

   qDebug() << "Lets connect things up to our new io device!";
}
