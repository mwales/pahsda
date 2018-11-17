#include "TrafficView.h"
#include "ui_TrafficView.h"

#include <QMessageBox>
#include <QtDebug>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QPluginLoader>
#include <QtSerialPort>
#include <QAbstractSocket>


#include "DataFrameFactoryInterface.h"

#include "PortChooser.h"

TrafficView::TrafficView(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::TrafficView),
   theInterface(nullptr),
   theInjectorService(this),
   theInjectorClient(nullptr)
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
   connect(ui->theProtocol, &QComboBox::currentTextChanged,
           this, &TrafficView::selectProtocol);

   loadPlugins();

   // Start injector service
   theInjectorService.setMaxPendingConnections(1);
   if (!theInjectorService.listen(QHostAddress::Any, 30003))
   {
      qDebug() << "Error listening on port 30003: " << theInjectorService.errorString();
   }
   else
   {
      qDebug() << "Listening on port " << theInjectorService.serverPort();

      connect(&theInjectorService, SIGNAL(newConnection()),
              this, SLOT(acceptInjectorConnection()));
   }


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
                            "Protocol Analyzer Highlighting Structured Data for Analysis\n"
                            "Version 0.1");
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
   pc.show();
   pc.exec();

   qDebug() << "Port chooser complete";

   theInterface = pc.getIoDevice();

   if (theInterface == nullptr)
   {
      qDebug() << "User didn't open anything...";
      return;
   }

   qDebug() << "Lets connect things up to our new io device!";

   if (theCurrentProtocol == nullptr)
   {
      QMessageBox::warning(this, "No protocol selected",
                           "You must select a protocol first before opening up a data connection");
      return;
   }

   // Connect data received to the protocol's pushMsgBytes method
   theInterface->setParent(this);
   connect(theInterface, &QSerialPort::readyRead,
           this, &TrafficView::ioReadReady);

   theInterface->open(QIODevice::ReadWrite);

   if (theInterface->bytesAvailable())
   {
      theCurrentProtocol->pushMsgBytes(theInterface->readAll());
   }
}

void TrafficView::selectProtocol(QString protocol)
{
   if (!theDataFrameFactories.contains(protocol))
   {
      QMessageBox::warning(this, "Invalid protocol selected", "Please pick a different protocol");
      clearFrames();
      theCurrentProtocol = nullptr;
      return;
   }

   // If the protocol hasn't really changed, just exit and do nothing
   if (theCurrentProtocol == theDataFrameFactories[protocol])
   {
      qDebug() << "Selected the same protocol that was already loaded";
      return;
   }

   // New protocol selected
   clearFrames();

   theCurrentProtocol = theDataFrameFactories[protocol];

   // Reconnect signals appropriately to route data

   qDebug() << theCurrentProtocol->statusToString();
}

void TrafficView::clearFrames()
{
   qDebug() << "Clear frames called - not implemented";
}

void TrafficView::ioReadReady()
{
   qDebug() << __PRETTY_FUNCTION__;

   if (theCurrentProtocol == nullptr)
   {
      qWarning() << "Received data, but no protocol configured to receive it";
   }
   else
   {
      theCurrentProtocol->pushMsgBytes(theInterface->readAll());
   }
}

void TrafficView::acceptInjectorConnection()
{
   if (theInjectorClient != nullptr)
   {
      qDebug() << "Dropping connection with existing injector client";
      theInjectorClient->close();
      theInjectorClient->deleteLater();
      theInjectorClient = nullptr;
   }

   if (!theInjectorService.hasPendingConnections())
   {
      qWarning() << "Received a signal that connection was ready, but no connections pending";
      qWarning() << " errorString = " << theInjectorService.errorString();
      return;
   }

   theInjectorClient = theInjectorService.nextPendingConnection();

   if (theInjectorClient == nullptr)
   {
      qWarning() << "nextPendingConnection returned a bad socket pointer";
      return;
   }

   connect(theInjectorClient, &QIODevice::readyRead,
           this, &TrafficView::injectorDataReady);
   connect(theInjectorClient, &QAbstractSocket::disconnected,
           this, &TrafficView::injectorClientDisconnected);
   connect(theInjectorClient, SIGNAL(error(QAbstractSocket::SocketError)),
           this, SLOT(injectorClientError(QAbstractSocket::SocketError)));

   qDebug() << "Accepted new connection from " << theInjectorClient->peerName();
}

void TrafficView::injectorDataReady()
{
   qDebug() << "Injector data ready";

   if (theInterface == nullptr)
   {
      qWarning() << "Injector trying to inject data, but no interface up yet";
      return;
   }

   QByteArray injectionData = theInjectorClient->readAll();

   int bytesWritten = theInterface->write(injectionData);
   if (bytesWritten == injectionData.length())
   {
      qDebug() << "Injecting " << bytesWritten << " bytes";
      qDebug() << " INJDATA: " << injectionData.toHex(' ');
   }
   else
   {
      qDebug() << "Failed to inject data.  Received " << injectionData.length()
               << " bytes to inject, wrote " << bytesWritten << " bytes";
   }

}

void TrafficView::injectorClientDisconnected()
{
   qDebug() << "Injector client disconnected";

   theInjectorClient->close();
   theInjectorClient->deleteLater();
   theInjectorClient = nullptr;
}

void TrafficView::injectorClientError(QAbstractSocket::SocketError error)
{
   qWarning() << "Injector client had an error" << error;
}

void TrafficView::loadPlugins()
{
   QDir directory("plugins");
   QStringList fileListing = directory.entryList(QDir::NoDotAndDotDot | QDir::Files);
   qDebug() << fileListing.join(" ");

   QStringList pluginListing;
   foreach(QString singleFile, fileListing)
   {
      if (singleFile.endsWith("plugin.so"))
      {
         qDebug() << "Has plugin name: " + singleFile;

         singleFile.prepend(QDir::separator());
         singleFile.prepend("plugins");
         pluginListing.append(singleFile);
      }
   }

   foreach(QString pluginFile, pluginListing)
   {
      QPluginLoader pluginLoader(pluginFile);

      QObject* po = pluginLoader.instance();

      DataFrameFactoryInterface* factory = qobject_cast<DataFrameFactoryInterface*>(po);

      if (factory == nullptr)
      {
         qDebug() << "Failed to load the plugin";
         qDebug() << pluginLoader.errorString();

      }
      else
      {
         qDebug() << "Successfully loaded the plugin";

         theDataFrameFactories.insert(factory->protocolName(), factory);

         ui->theProtocol->addItem(factory->protocolName());
      }
   }

}
