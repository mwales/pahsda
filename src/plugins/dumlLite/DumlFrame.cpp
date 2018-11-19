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

   addField(DUML_SOF,               "Start of Frame",  "SOF");
   addField(DUML_FRAME_LEN,         "Frame Length",    "Len");
   addField(DUML_FRAME_HEADER_CRC8, "Header CRC-8",    "CRC8");
   addField(DUML_SOURCE,            "Source",          "SRC");
   addField(DUML_TARGET,            "Target",          "TGT");
   addField(DUML_SEQUENCE,          "Sequence Number", "SEQ");
   addField(DUML_FLAGS,             "Flags",           "FLG");
   addField(DUML_COMMAND_SET,       "Command Set",     "SET");
   addField(DUML_COMMAND_ID,        "Command ID",      "ID");
   addField(DUML_PAYLOAD,           "Payload",         "Payload");
   addField(DUML_FRAME_CRC16,       "Frame CRC-16",    "CRC16");

   QList<int> sortingOrder;
   sortingOrder.append(DUML_COMMAND_SET);
   sortingOrder.append(DUML_COMMAND_ID);
   sortingOrder.append(DUML_TARGET);
   sortingOrder.append(DUML_SOURCE);
   setSortingIndexes(sortingOrder);

   addValue(DUML_SOF,               framedData.mid(0,1));
   addValue(DUML_FRAME_LEN,         framedData.mid(1,2));
   addValue(DUML_FRAME_HEADER_CRC8, framedData.mid(3,1));
   addValue(DUML_SOURCE,            framedData.mid(4,1));
   addValue(DUML_TARGET,            framedData.mid(5,1));
   addValue(DUML_SEQUENCE,          framedData.mid(6,2));
   addValue(DUML_FLAGS,             framedData.mid(8,1));
   addValue(DUML_COMMAND_SET,       framedData.mid(9,1));
   addValue(DUML_COMMAND_ID,        framedData.mid(10,1));
   addValue(DUML_PAYLOAD,           framedData.mid(11, framedData.length() - 13));
   addValue(DUML_FRAME_CRC16,       framedData.right(2));
}
