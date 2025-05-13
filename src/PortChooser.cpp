#include "PortChooser.h"
#include "ui_PortChooser.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QtDebug>

const QString LOCATION_LABEL = "Location: ";
const QString MFG_LABEL = "Manufacturer: ";

PortChooser::PortChooser(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::PortChooser),
   theOpenSerialPort(nullptr)
{
   ui->setupUi(this);

   refreshPortListing();

   connect(ui->theRefreshButton, &QPushButton::clicked,
           this, &PortChooser::refreshPortListing);
   connect(ui->thePortName, &QComboBox::currentTextChanged,
           this, &PortChooser::updatePortExtraInfo);
   connect(ui->theOpenButton, &QPushButton::clicked,
           this, &PortChooser::openSerialPort);
}

void PortChooser::refreshPortListing()
{
   qDebug() << "Refreshing the serial port listing";

   QString currentSelectedPort = ui->thePortName->currentText();

   thePortList = QSerialPortInfo::availablePorts();

   // Replace everything in the list with the newly returned data
   while(ui->thePortName->count())
   {
      ui->thePortName->removeItem(0);
   }

   int i = 0;
   foreach(const QSerialPortInfo& spi, thePortList)
   {
      ui->thePortName->addItem(spi.portName());

      if (spi.portName() == currentSelectedPort)
      {
         // Select this item again since it was selected before refresh pressed
         ui->thePortName->setCurrentIndex(i);
      }

      i++;
   }
}

void PortChooser::updatePortExtraInfo(QString curText)
{
   // Can we find which item in the port list is the one the user selected..
   for(int i = 0; i < thePortList.count(); i++)
   {
      if (thePortList[i].portName() == curText)
      {
         // We found the port
         ui->theLocation->setText(LOCATION_LABEL + thePortList[i].systemLocation());
         ui->theMfgLabel->setText(MFG_LABEL + thePortList[i].manufacturer());

         return;
      }
   }

   // If we got here, it means that the serial port name doens't match any in the list
   ui->theLocation->setText(LOCATION_LABEL);
   ui->theMfgLabel->setText(MFG_LABEL);
}

void PortChooser::openSerialPort()
{
   // If the name of the serial port matches one in the internal list, use the system path from the
   // object, else, use the name as the user typed it
   QString serialPort = ui->thePortName->currentText();
   foreach(const QSerialPortInfo& spi, thePortList)
   {
      if (spi.portName() == serialPort)
      {
         // Get the path from the QSerialPortInfo object
         serialPort = spi.portName();
         break;
      }
   }

   qDebug() << "Opening serial port " << serialPort;

   QSerialPort* sp = new QSerialPort(serialPort);
   sp->setBaudRate(getBaudRate(), QSerialPort::AllDirections);
   sp->setDataBits(getDataBits());
   sp->setParity(getParity());
   sp->setStopBits(getStopBits());
   sp->setFlowControl(getFlowControl());
   theOpenSerialPort = sp;

   this->close();
}

PortChooser::~PortChooser()
{
   delete ui;
}

QIODevice* PortChooser::getIoDevice()
{
   return theOpenSerialPort;
}

enum QSerialPort::BaudRate PortChooser::getBaudRate()
{
    switch(ui->theBaudRateCb->currentIndex())
        {
        case 0:
            qDebug() << "Baudrate 1200";
            return QSerialPort::Baud1200;
        case 1:
            qDebug() << "Baudrate 2400";
            return QSerialPort::Baud2400;
        case 2:
            qDebug() << "Baudrate 4800";
            return QSerialPort::Baud4800;
        case 3:
            qDebug() << "Baudrate 9600";
            return QSerialPort::Baud9600;
        case 4:
            qDebug() << "Baudrate 19200";
            return QSerialPort::Baud19200;
        case 5:
            qDebug() << "Baudrate 38400";
            return QSerialPort::Baud38400;
        case 6:
            qDebug() << "Baudrate 57600";
            return QSerialPort::Baud57600;
        case 7:
            qDebug() << "Baudrate 115200";
            return QSerialPort::Baud115200;
        default:
            qWarning() << "Unknown Baudrate selection, returning 9600";
            return QSerialPort::Baud9600;
        }
}


enum QSerialPort::DataBits PortChooser::getDataBits()
{
    switch(ui->theDataBitsCb->currentIndex())
        {
        case 0:
            qDebug() << "5 Data Bits";
            return QSerialPort::Data5;
        case 1:
            qDebug() << "6 Data Bits";
            return QSerialPort::Data6;
        case 2:
            qDebug() << "7 Data Bits";
            return QSerialPort::Data7;
        case 3:
            qDebug() << "8 Data Bits";
            return QSerialPort::Data8;
        default:
            qDebug() << "Unknown Data Bits selection return 8 bits";
            return QSerialPort::Data8;
        }
}

enum QSerialPort::FlowControl PortChooser::getFlowControl()
{
	switch(ui->theFlowControlCb->currentIndex())
	{
	case 0:
		qDebug() << "QSerialPort::NoFlowControl";
		return QSerialPort::NoFlowControl;
	case 1:
		qDebug() << "QSerialPort::HardwareControl";
		return QSerialPort::HardwareControl;
	case 2:
		qDebug() << "QSerialPort::SoftwareControl";
		return QSerialPort::SoftwareControl;
	default:
		qDebug() << "Unknown Flow control selection return NoFlowControl";
		return QSerialPort::NoFlowControl;
	}
}

enum QSerialPort::Parity PortChooser::getParity()
{
	switch(ui->theParityCb->currentIndex())
	{
	case 0:
		qDebug() << "QSerialPort::NoParity";
		return QSerialPort::NoParity;
	case 1:
		qDebug() << "QSerialPort::EvenParity";
		return QSerialPort::EvenParity;
	case 2:
		qDebug() << "QSerialPort::OddParity";
		return QSerialPort::OddParity;
	case 3:
		qDebug() << "QSerialPort::SpaceParity";
		return QSerialPort::SpaceParity;
	case 4:
		qDebug() << "QSerialPort::MarkParity";
		return QSerialPort::MarkParity;
	default:
		qDebug() << "Unknown Parity selection return No Parity";
		return QSerialPort::NoParity;
	}
}

enum QSerialPort::StopBits PortChooser::getStopBits()
{
	switch(ui->theStopBitsCb->currentIndex())
	{
	case 0:
		qDebug() << "QSerialPort::OneStop";
		return QSerialPort::OneStop;
	case 1:
		qDebug() << "QSerialPort::OneAndHalfStop";
		return QSerialPort::OneAndHalfStop;
	case 2:
		qDebug() << "QSerialPort::TwoStop";
		return QSerialPort::TwoStop;
	default:
		qDebug() << "Unknown Stop Bits selection return 1 stop bit";
		return QSerialPort::OneStop;
	}
}

