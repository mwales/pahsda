#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QObject>
#include "DataFrameFactoryInterface.h"


class TestPlugin: public QObject, public DataFrameFactoryInterface
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID DataFactoryInterface_iid)
   Q_INTERFACES(DataFrameFactoryInterface)

public:
   TestPlugin();

   virtual ~TestPlugin();

   virtual void pushMsgBytes(QByteArray msgData);

   virtual bool isFrameReady();

   virtual DataFrame* getNextFrame();

   virtual QString statusToString();

   virtual QString protocolName();

protected:

   QByteArray theDataBytes;

   int theFrameCount;

   int theTotalBytesReceived;

   int theTotalBytesFramed;
};

#endif // TESTPLUGIN_H
