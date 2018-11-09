#ifndef DATAFRAMEFACTORYINTERFACE_H
#define DATAFRAMEFACTORYINTERFACE_H

#include <QtPlugin>

#include <QString>
#include <QByteArray>
#include "DataFrame.h"

#define DataFactoryInterface_iid "pahsda.DataFactoryInterface"

/**
 * Main interface for Qt plugins for different protocols
 */
class DataFrameFactoryInterface
{

public:

   virtual ~DataFrameFactoryInterface() {}

   virtual void pushMsgBytes(QByteArray msgData) = 0;

   virtual bool isFrameReady() = 0;

   virtual DataFrame* getNextFrame() = 0;

   virtual QString statusToString() = 0;

   virtual QString protocolName() = 0;

};

Q_DECLARE_INTERFACE(DataFrameFactoryInterface, DataFactoryInterface_iid)

#endif // DATAFRAMEFACTORYINTERFACE_H
