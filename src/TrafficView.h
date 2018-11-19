#ifndef TRAFFICVIEW_H
#define TRAFFICVIEW_H

#include <QMainWindow>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLinkedList>

#include "DataFrame.h"

class DataFrameFactoryInterface;

namespace Ui {
class TrafficView;
}

/**
 * Main GUI.  Hope to show the messages as they come in.
 *
 * @todo Display framed data using the DataFrameDisplay class
 * @todo Log the data?
 */
class TrafficView : public QMainWindow
{
   Q_OBJECT

public:
   explicit TrafficView(QWidget *parent = nullptr);
   ~TrafficView();

private slots:

   void showAbout();

   void openSerialPort();

   void closeInterface();

   void selectProtocol(QString protocol);

   void clearFrames();

   void ioReadReady();

   void acceptInjectorConnection();

   void injectorDataReady();

   void injectorClientDisconnected();

   void injectorClientError(QAbstractSocket::SocketError error);

private:

   void loadPlugins();

   void addFrame(DataFrame* frame);

   void displayFrame(DataFrame* frame, int row);

   Ui::TrafficView *ui;

   QIODevice* theInterface;

   QMap<QString, DataFrameFactoryInterface*> theDataFrameFactories;

   DataFrameFactoryInterface* theCurrentProtocol;

   QTcpServer theInjectorService;

   QTcpSocket* theInjectorClient;

   /// @todo Is the inserting and replacing code I have using this container really working OK,
   /// consider just converting to a QVector for simplicity
   QLinkedList<DataFrame*> theFrames;



};

#endif // TRAFFICVIEW_H
