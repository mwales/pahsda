#include "DumlLitePlugin.h"

#include <QtDebug>

// Uncomment to enable debugging of this class
// #define DUML_LITE_FF_DEBUG

#ifdef DUML_LITE_FF_DEBUG
   #define dlDebug qDebug
   #define dlWarning qWarning
#else
   #define dlDebug if(0) qDebug
   #define dlWarning if(0) qWarning
#endif

DumlLiteFrameFactory::DumlLiteFrameFactory():
   theFrameCount(0),
   theTotalBytesReceived(0),
   theTotalBytesFramed(0)
{
   dlDebug() << "Duml Lite frame factory created";
}

DumlLiteFrameFactory::~DumlLiteFrameFactory()
{
   dlDebug() << "Duml Lite frame factory deleted";
}

/**
 *
 * DUML Framing Format
 * Byte 0 = Start of Frame = 0x55
 * Byte 1/2 = DUML Version and Message Length.
 *   Bits 0xFC00 = DUML version
 *   Bits 0x03FF = Length (max 1024 bytes) (Number of bytes including SOF and ending checksum)
 * Byte 3 = crc8 (of bytes 0-2)
 *
 *   Bytes 4 - N-2 = DUML Framed Data
 *
 * Bytes N-2/N-1 = crc16
 *
 * DUML Payload bytes
 *
 * Byte 4 = Source and index
 *   Bits 0xE0 = source index
 *   Bits 0x1F = source
 * Byte 5 = Target and index
 *   Bits 0xE0 = target index
 *   Bits 0x1F = target
 * Bytes 6/7 = Sequence Number
 *
 * Byte 8 = Flags
 * Byte 9 = Command Set Number:
 * Byte 10 = Command ID
 */
void DumlLiteFrameFactory::pushMsgBytes(QByteArray msgData)
{
   theTotalBytesReceived += msgData.length();
   theDataBytes.append(msgData);

   dlDebug() << __PRETTY_FUNCTION__ << " received " << msgData.length() << "bytes of data";
   // dlDebug() << "WHOLE RAW: " << msgData.toHex(' ');

   findFrames();

}

void DumlLiteFrameFactory::findFrames()
{
   while(findNextFrame())
   {
      theFrameCount++;
   }
}

/**
 * @todo Check DUML header crc8
 * @todo Check DUML frame crc16
 */
bool DumlLiteFrameFactory::findNextFrame()
{
   // Find the start of frame
   int startOfFrame = -1;
   for(int i = 0; i < theDataBytes.length(); i++)
   {
      // Search for the start of frame marker
      if (theDataBytes.at(i) == 0x55)
      {
         // We found the start of frame marker
         startOfFrame = i;
         break;
      }
   }

   if (startOfFrame == -1)
   {
      // Could not find any start of frame marker, flushing the data
      dlDebug() << "Discarding all received data: " << theDataBytes.length() << " bytes";
      theDataBytes.clear();
      return false;
   }

   if (startOfFrame != 0)
   {
      // There was some unframed data on the beginning of the buffer, discard
      dlDebug() << "Discarding received data before SOF: " << startOfFrame << " bytes";
      theDataBytes.remove(0, startOfFrame);
   }

   // At this point, the first byte of data in the frame is SOF
   if (theDataBytes.length() < 13)
   {
      dlDebug() << "We have the beginning of the frame, but it is not complete yet, only "
                << theDataBytes.length() << " bytes received so far";
      return false;
   }

   int dumlFrameLength = ( theDataBytes[2] << 8) | theDataBytes[1];
   dumlFrameLength &= 0x03ff;

   if (dumlFrameLength > theDataBytes.length())
   {
      dlDebug() << "We have DUML Frame Header, DUML length = " << dumlFrameLength
                << ", but we only have " << theDataBytes.length() << " bytes in buffer";
      return false;
   }

   dlDebug() << "DUML Frame Length = " << dumlFrameLength;


   QByteArray framedData = theDataBytes.left(dumlFrameLength);
   theDataBytes.remove(0, dumlFrameLength);

   dlDebug() << "RAW: " << framedData.toHex(' ');

   DumlFrame* df = new DumlFrame(framedData);
   theFrames.append(df);

   theTotalBytesFramed += dumlFrameLength;

   return true;
}


bool DumlLiteFrameFactory::isFrameReady()
{
   dlDebug() << __PRETTY_FUNCTION__ << ( theFrames.empty() ? ": NO" : ": YES");
   return !theFrames.empty();

}

DataFrame* DumlLiteFrameFactory::getNextFrame()
{
   dlDebug() << __PRETTY_FUNCTION__ << " called";

   if (isFrameReady())
   {
      DataFrame* retVal = theFrames.front();
      theFrames.pop_front();
      return retVal;
   }
   else
   {
      dlWarning() << "Asked for a frame, but we have none!";
      return nullptr;
   }
}

QString DumlLiteFrameFactory::statusToString()
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

QString DumlLiteFrameFactory::protocolName()
{
   return "DUML Lite";
}
