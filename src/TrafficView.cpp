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
#include <QLabel>

// Uncomment to enable debugging of this class
#define TRAFFIC_VIEW_DEBUG

#ifdef TRAFFIC_VIEW_DEBUG
   #define tvDebug qDebug
   #define tvWarning qWarning
#else
   #define tvDebug if(0) qDebug
   #define tvWarning if(0) qWarning
#endif

// Uncomment to enable debugging of this class
// #define TRAFFIC_VIEW_VERBOSE_DEBUG

#ifdef TRAFFIC_VIEW_VERBOSE_DEBUG
   #define vtvDebug qDebug
#else
   #define vtvDebug if(0) qDebug
#endif

#include "DataFrameFactoryInterface.h"

#include "PortChooser.h"
#include "Helpers.h"

TrafficView::TrafficView(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::TrafficView),
   theInterface(nullptr),
   theCurrentProtocol(nullptr),
   theInjectorService(this),
   theInjectorClient(nullptr)
{
   ui->setupUi(this);

   theUpdateStatusTimer.setInterval(1000);

   connect(ui->actionAbout, &QAction::triggered,
           this, &TrafficView::showAbout);
   connect(ui->actionAbout_Qt, &QAction::triggered,
           this, &TrafficView::showAboutQt);
   connect(ui->actionOpen_Serial_Port, &QAction::triggered,
           this, &TrafficView::openSerialPort);
   connect(ui->theProtocol, &QComboBox::currentTextChanged,
           this, &TrafficView::selectProtocol);
   connect(ui->theCloseButton, &QPushButton::clicked,
           this, &TrafficView::closeInterface);
   connect(&theUpdateStatusTimer, &QTimer::timeout,
           this, &TrafficView::statusUpdateTimerFired);

   loadPlugins();

   // Start injector service
   theInjectorService.setMaxPendingConnections(1);
   if (!theInjectorService.listen(QHostAddress::Any, 30003))
   {
      tvDebug() << "Error listening on port 30003: " << theInjectorService.errorString();
   }
   else
   {
      tvDebug() << "Listening on port " << theInjectorService.serverPort();

      connect(&theInjectorService, SIGNAL(newConnection()),
              this, SLOT(acceptInjectorConnection()));
   }

   theUpdateStatusTimer.start();

   ui->theFrameView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

TrafficView::~TrafficView()
{
   delete ui;

   if (theInterface != nullptr)
   {
      tvDebug() << "Closing the io device before we exit";
      theInterface->close();
   }

   clearFrames();
}

void TrafficView::showAbout()
{
   tvDebug() << "Showing about";
   QMessageBox::about(this,
                      "About PAHSDA",
                      "PAHSDA\n"
                      "Protocol Analyzer Highlighting Structured Data for Analysis\n"
                      "Version 0.1");
}

void TrafficView::showAboutQt()
{
   tvDebug() << "Showing about Qt";
   QMessageBox::aboutQt(this,
                        "About Qt");
}

void TrafficView::openSerialPort()
{
   if (theInterface != nullptr)
   {
      tvDebug() << "Closing the old interface before propmting user to choose new one";
      theInterface->close();

      theInterface->deleteLater();

      theInterface = nullptr;
   }

   PortChooser pc(this);
   pc.show();
   pc.exec();

   tvDebug() << "Port chooser complete";

   theInterface = pc.getIoDevice();

   if (theInterface == nullptr)
   {
      tvDebug() << "User didn't open anything...";
      return;
   }

   tvDebug() << "Lets connect things up to our new io device!";

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

void TrafficView::closeInterface()
{
   tvDebug() << "Close button pressed";

   if (theInterface == nullptr)
   {
      // Do nothing
      return;
   }

   theInterface->close();
   theInterface->deleteLater();
   theInterface = nullptr;

   tvDebug() << "Status of protocol:\n" << theCurrentProtocol->statusToString();
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
      tvDebug() << "Selected the same protocol that was already loaded";
      return;
   }

   // New protocol selected
   clearFrames();

   theCurrentProtocol = theDataFrameFactories[protocol];

   // Reconnect signals appropriately to route data

   tvDebug() << theCurrentProtocol->statusToString();
}

void TrafficView::clearFrames()
{
   tvDebug() << "Clear frames called";

   // Clear all the data from the frame view
   ui->theFrameView->setRowCount(0);

   foreach(DataFrame* df, theFrames)
   {
      delete df;
   }
}

void TrafficView::ioReadReady()
{
   vtvDebug() << __PRETTY_FUNCTION__;

   if (theCurrentProtocol == nullptr)
   {
      tvWarning() << "Received data, but no protocol configured to receive it";
      return;
   }

   theCurrentProtocol->pushMsgBytes(theInterface->readAll());

   while(theCurrentProtocol->isFrameReady())
   {
      addFrame(theCurrentProtocol->getNextFrame());
   }

   // vtvDebug() << "Going to dump the contents of all the frames:";
   // foreach(DataFrame* df, theFrames)
   // {
   //    tvDebug() << df->toString();
   // }
}

/**
 * This function needs to take ownership of all frames that it receives.  If it doesn't want them
 * it needs to delete them
 */
void TrafficView::addFrame(DataFrame* frame)
{
   // Special case, is there no frames so far?
   if (theFrames.empty())
   {
      tvDebug() << "Adding first frame, formatting the headers";
      // No frames have ever been received, use this frame to format the table!

      // Add the header
      QList<int> fields = frame->getFieldIndexes();
      ui->theFrameView->setColumnCount(fields.length());

      QStringList headerStrings;
      foreach(int fieldIndex, fields)
      {
         headerStrings.append(frame->getFieldAbbrev(fieldIndex));
      }
      ui->theFrameView->setHorizontalHeaderLabels(headerStrings);

      theFrames.push_back(frame);

      // Frames that we keep will be deleted during cleanup of the GUI
      addFrameToTable(frame, 0);

      return;
   }

   // Find out where the new frame goes in our ordered list
   int row = 0;
   for(auto oldFrameIter = theFrames.begin(); oldFrameIter != theFrames.end(); oldFrameIter++)
   {
      if (*frame > **oldFrameIter)
      {
         // Not equal, keep iterating!
         row++;
         continue;
      }

      if (*frame == **oldFrameIter)
      {
         // This is the same message type, lets just update the old frame
         vtvDebug() << "Update the old frame here!";

         (*oldFrameIter)->updateFrame(*frame);

         // Delete the new frame that was just created since we are reusing the old frame
         delete frame;
         return;
      }

      vtvDebug() << "Inserting frame in the middle at row " << row;
      // If we got here, we need to insert the new frame here
      theFrames.insert(oldFrameIter, frame);
      ui->theFrameView->insertRow(row);
      addFrameToTable(frame, row);
      return;
   }

   // We must be the last item to be added
   vtvDebug() << "Inserting as the new last frame";
   theFrames.push_back(frame);
   addFrameToTable(frame, theFrames.count() - 1);
}

void TrafficView::addFrameToTable(DataFrame* frame, int row)
{
   if (row >= ui->theFrameView->rowCount())
   {
      ui->theFrameView->setRowCount(row + 1);
   }

   QList<int> fields = frame->getFieldIndexes();
   int col = 0;
   foreach(int fieldIndex, fields)
   {
      // tvDebug() << "Item(row=" << row << ", col=" << col << ") = " << frame->getFieldValueString(fieldIndex);
      QLabel* fieldDataLabel = frame->getLabel(fieldIndex, this);

      if (fieldDataLabel == nullptr)
      {
         ui->theFrameView->setCellWidget(row, col++, new QLabel("NULLPTR"));
      }
      else
      {
         ui->theFrameView->setCellWidget(row, col++, fieldDataLabel);
      }
   }


}

void TrafficView::acceptInjectorConnection()
{
   if (theInjectorClient != nullptr)
   {
      tvDebug() << "Dropping connection with existing injector client";
      theInjectorClient->close();
      theInjectorClient->deleteLater();
      theInjectorClient = nullptr;
   }

   if (!theInjectorService.hasPendingConnections())
   {
      tvWarning() << "Received a signal that connection was ready, but no connections pending";
      tvWarning() << " errorString = " << theInjectorService.errorString();
      return;
   }

   theInjectorClient = theInjectorService.nextPendingConnection();

   if (theInjectorClient == nullptr)
   {
      tvWarning() << "nextPendingConnection returned a bad socket pointer";
      return;
   }

   connect(theInjectorClient, &QIODevice::readyRead,
           this, &TrafficView::injectorDataReady);
   connect(theInjectorClient, &QAbstractSocket::disconnected,
           this, &TrafficView::injectorClientDisconnected);
   connect(theInjectorClient, SIGNAL(error(QAbstractSocket::SocketError)),
           this, SLOT(injectorClientError(QAbstractSocket::SocketError)));

   tvDebug() << "Accepted new connection from " << theInjectorClient->peerName();
}

void TrafficView::injectorDataReady()
{
   tvDebug() << "Injector data ready";

   if (theInterface == nullptr)
   {
      tvWarning() << "Injector trying to inject data, but no interface up yet";
      return;
   }

   QByteArray injectionData = theInjectorClient->readAll();

   long long bytesWritten = theInterface->write(injectionData);
   if (bytesWritten == injectionData.length())
   {
      tvDebug() << "Injecting " << bytesWritten << " bytes";
      tvDebug() << " INJDATA: " << Helpers::qbyteToHexString(injectionData);
   }
   else
   {
      tvDebug() << "Failed to inject data.  Received " << injectionData.length()
               << " bytes to inject, wrote " << bytesWritten << " bytes";
   }

}

void TrafficView::injectorClientDisconnected()
{
   tvDebug() << "Injector client disconnected";

   theInjectorClient->close();
   theInjectorClient->deleteLater();
   theInjectorClient = nullptr;
}

void TrafficView::injectorClientError(QAbstractSocket::SocketError error)
{
   tvWarning() << "Injector client had an error" << error;
}

void TrafficView::statusUpdateTimerFired()
{
   if (theCurrentProtocol != nullptr)
   {
      ui->statusBar->showMessage(theCurrentProtocol->statusToString(), 1000);
   }
}

void TrafficView::loadPlugins()
{
   QDir directory("plugins");
   QStringList fileListing = directory.entryList(QDir::NoDotAndDotDot | QDir::Files);
   tvDebug() << fileListing.join(" ");

   QStringList pluginListing;
   foreach(QString singleFile, fileListing)
   {
      if (singleFile.endsWith("plugin.so"))
      {
         tvDebug() << "Has plugin name: " + singleFile;

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
         tvDebug() << "Failed to load the plugin";
         tvDebug() << pluginLoader.errorString();

      }
      else
      {
         tvDebug() << "Successfully loaded the plugin";

         theDataFrameFactories.insert(factory->protocolName(), factory);

         ui->theProtocol->addItem(factory->protocolName());
      }
   }

}
