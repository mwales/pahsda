#ifndef PORTCHOOSER_H
#define PORTCHOOSER_H

#include <QDialog>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QList>
#include <QIODevice>


namespace Ui {
class PortChooser;
}

/**
 * Lets the user pick a serial port to open
 *
 * @todo Select a baud rate to configure the port for
 * @todo Support other serial port configuration parameters
 * @todo Create a parent type IoDeviceChooser, create a socket based child class
 */
class PortChooser : public QDialog
{
   Q_OBJECT

public:
   explicit PortChooser(QWidget *parent = nullptr);
   ~PortChooser();

   QIODevice* getIoDevice();

   enum QSerialPort::BaudRate getBaudRate();

   enum QSerialPort::DataBits getDataBits();

   enum QSerialPort::FlowControl getFlowControl();

   enum QSerialPort::Parity getParity();

   enum QSerialPort::StopBits getStopBits();

private slots:

   void refreshPortListing();

   void updatePortExtraInfo(QString curText);

   void openSerialPort();

private:

   Ui::PortChooser *ui;

   QList<QSerialPortInfo> thePortList;

   QIODevice* theOpenSerialPort;
};

#endif // PORTCHOOSER_H
