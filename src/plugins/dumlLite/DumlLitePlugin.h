#ifndef DUML_LITE_FRAME_FACTORY_H
#define DUML_LITE_FRAME_FACTORY_H

#include <QObject>
#include <QList>

#include "DataFrameFactoryInterface.h"
#include "DumlFrame.h"
/**
 * Plugin for PAHSDA
 *
 * Decodes data stream into DUML Frames
 */
class DumlLiteFrameFactory: public QObject, public DataFrameFactoryInterface
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID DataFactoryInterface_iid)
   Q_INTERFACES(DataFrameFactoryInterface)

public:
   DumlLiteFrameFactory();

   virtual ~DumlLiteFrameFactory();

   virtual void pushMsgBytes(QByteArray msgData);

   virtual bool isFrameReady();

   virtual DataFrame* getNextFrame();

   virtual QString statusToString();

   virtual QString protocolName();

protected:

   void findFrames();

   bool findNextFrame();

   QByteArray theDataBytes;

   int theFrameCount;

   int theTotalBytesReceived;

   int theTotalBytesFramed;

   QList<DumlFrame*> theFrames;
};

#endif // DUML_LITE_FRAME_FACTORY_H
