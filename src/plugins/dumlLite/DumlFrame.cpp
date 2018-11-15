#include "DumlFrame.h"

#include <QtDebug>
#include <QByteArray>

// Uncomment to enable debugging of this class
#define DUML_FRAME_DEBUG

#ifdef DUML_FRAME_DEBUG
   #define dFrameDebug qDebug
   #define dFrameWarning qWarning
#else
   #define dFrameDebug if(0) qDebug
   #define dFrameWarning if(0) qWarning
#endif

DumlFrame::DumlFrame(QByteArray framedData)
{
   dFrameDebug() << "Creating a DUML Frame";
}
