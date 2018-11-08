#include "PortChooser.h"
#include "ui_PortChooser.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QtDebug>

#include "Bad.h"

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

   theOpenSerialPort = new QSerialPort(serialPort);

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
