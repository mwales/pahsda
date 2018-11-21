#include "TestPlugin.h"

#include <QtDebug>

TestPlugin::TestPlugin():
   theFrameCount(0),
   theTotalBytesReceived(0),
   theTotalBytesFramed(0)
{
   qDebug() << "DataFrameFactory constructor called";
}

TestPlugin::~TestPlugin()
{
   qDebug() << "TestPlugin deleted";
}

void TestPlugin::pushMsgBytes(QByteArray msgData)
{
   qWarning() << __PRETTY_FUNCTION__ << " called, but no implementation!";
   Q_UNUSED(msgData);
}

bool TestPlugin::isFrameReady()
{
   qWarning() << __PRETTY_FUNCTION__ << " called, but no implementation!";
   return false;

}

DataFrame* TestPlugin::getNextFrame()
{
   qWarning() << __PRETTY_FUNCTION__ << " called, but no implementation!";
   return nullptr;
}

QString TestPlugin::statusToString()
{
   QString retVal;
   retVal += "BytesRxed=";
   retVal += QString::number(theTotalBytesReceived);
   retVal += ", BytesFramed=";
   retVal += QString::number(theTotalBytesFramed);
   retVal += ", FrameCount=";
   retVal += QString::number(theFrameCount);
   retVal += ", BytesBuffered=";
   retVal += QString::number(theDataBytes.length());
   retVal += ", BytesDiscarded=";
   retVal += QString::number(theTotalBytesReceived - theTotalBytesFramed - theDataBytes.length());
   return retVal;
}

QString TestPlugin::protocolName()
{
   return "Test Plugin";
}
