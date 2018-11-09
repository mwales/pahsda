#ifndef TRAFFICVIEW_H
#define TRAFFICVIEW_H

#include <QMainWindow>
#include <QMap>

class DataFrameFactoryInterface;

namespace Ui {
class TrafficView;
}

/**
 * Main GUI.  Hope to show the messages as they come in.
 *
 * @todo Connect and receive incoming data
 * @todo Send incoming data the DataFramingFactory
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

   void selectProtocol(QString protocol);

   void clearFrames();

private:

   void loadPlugins();

   Ui::TrafficView *ui;

   QIODevice* theInterface;

   QMap<QString, DataFrameFactoryInterface*> theDataFrameFactories;

   DataFrameFactoryInterface* theCurrentProtocol;


};

#endif // TRAFFICVIEW_H
