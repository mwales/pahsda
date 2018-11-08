#ifndef TRAFFICVIEW_H
#define TRAFFICVIEW_H

#include <QMainWindow>

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

private:
   Ui::TrafficView *ui;

   QIODevice* theInterface;
};

#endif // TRAFFICVIEW_H
