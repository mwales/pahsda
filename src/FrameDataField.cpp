#include "FrameDataField.h"

#include <QLabel>

#include "Helpers.h"

#include <QtDebug>

// Uncomment to enable debugging of this class
// #define FRAME_DATA_FIELD_DEBUG

#ifdef FRAME_DATA_FIELD_DEBUG
   #define fdfDebug qDebug
   #define fdfWarning qWarning
#else
   #define fdfDebug if(0) qDebug
   #define fdfWarning if(0) qWarning
#endif

#define FRAMER_TIMER_TICK_DURATION_MS 500

const QStringList FrameDataField::BACKGROUND_SHADES = QStringList() << "#ffffff" // lightest / white
                                                                    << "#e6e6ff"
                                                                    << "#ccccff"
                                                                    << "#b3b3ff"
                                                                    << "#9999ff"
                                                                    << "#8080ff"
                                                                    << "#6666ff"
                                                                    << "#4d4dff"
                                                                    << "#3333ff";
                                                                    //<< "#1a1aff"
                                                                    //<< "#0000ff"
                                                                    //<< "#0000e6"
                                                                    //<< "#0000cc"
                                                                    //<< "#0000b3"; // darkest / blue

const int FrameDataField::NUM_SHADES = FrameDataField::BACKGROUND_SHADES.length();

FrameDataField::FrameDataField(QObject* parent):
   QObject(parent),
   theLabel(nullptr),
   theHighlightInterval(0),
   theOptionsFlag(0),
   theUpdateTimer(this)
{
   theUpdateTimer.setSingleShot(true);

   connect(&theUpdateTimer, &QTimer::timeout,
           this, &FrameDataField::timerTick);
}

void FrameDataField::setHighlightDuration(int timerTicks)
{
   theHighlightInterval = timerTicks;

   theStyleString = "<style>\n";

   for(int i = timerTicks; i >= 0; i--)
   {
      theStyleString += "  .fdf";
      theStyleString += QString::number(i);
      theStyleString += " { background-color: ";
      theStyleString += FrameDataField::BACKGROUND_SHADES[colorIndexLookup(i)];
      theStyleString += " } \n";
   }

   theStyleString += "</style>";
}

// // sert timer on upate??   @todotoday
void FrameDataField::updateValue(QByteArray data)
{
   // fdfDebug() << __PRETTY_FUNCTION__;

   if (!theHighlightInterval)
   {
      // Highlighting has not been configured for this data field
      fdfDebug() << "No highlighting.  OLD=" << theData.toHex() << " NEW=" << data.toHex();
      theData = data;

      if (theLabel != nullptr)
      {
         theLabel->setText(getFieldValueRichString());
      }

      return;
   }

   fdfDebug() << "Highlighting on.  OLD=" << theData.toHex() << " NEW=" << data.toHex();


   // Incase the field length has changed, only use the shortest field length
   int numBytes = theData.length();
   if (numBytes > data.length())
   {
      numBytes = data.length();
   }

   bool changeFound = false;
   for(int i = 0; i < numBytes; i++)
   {
      if (data[i] != theData[i])
      {
         theBytesHighlighted[i] = theHighlightInterval;
         changeFound = true;
      }
   }

   theData = data;

   if (theLabel != nullptr)
   {
      theLabel->setText(getFieldValueRichString());
      fdfDebug() << "Label: " << getFieldValueRichString();
   }

   if (changeFound && !theUpdateTimer.isActive())
   {
      fdfDebug() << "Starting the timer";
      theUpdateTimer.start(FRAMER_TIMER_TICK_DURATION_MS);
   }
}

void FrameDataField::setValue(QByteArray data)
{
   theData = data;
}

QLabel* FrameDataField::getLabel(QObject* parentParam)
{
   if (parent() == nullptr)
   {
      setParent(parentParam);
      theUpdateTimer.setParent(parentParam);
   }

   if (theLabel == nullptr)
   {
      // If highlighting is turned on, highlight everything on a fresh frame!
      if (theHighlightInterval)
      {
         for(int i = 0; i < theData.length(); i++)
         {
            theBytesHighlighted[i] = theHighlightInterval;
         }

         if (!theUpdateTimer.isActive())
         {
            fdfDebug() << "Starting the timer (fresh label created)";
            theUpdateTimer.start(FRAMER_TIMER_TICK_DURATION_MS);
         }
      }

      theLabel = new QLabel();
      theLabel->setTextFormat(Qt::RichText);
      theLabel->setText(getFieldValueRichString());
      return theLabel;
   }
   else
   {
      return theLabel;
   }
}

QByteArray FrameDataField::getFieldRawValue()
{
   return theData;
}

QString FrameDataField::getFieldValueString()
{
   if (theOptionsFlag & ASCII_DISPLAY)
   {
      QString retVal;

      for(int i = 0; i < theData.length(); i++)
      {
         retVal.append(readableAsciiConversion(static_cast<char>(theData.at(i))));
      }

      return retVal;
   }
   else
   {
      return Helpers::qbyteToHexString(theData);
   }
}

QString FrameDataField::getFieldValueRichString()
{
   QString retVal = theStyleString;

   for(int i = 0; i < theData.length(); i++)
   {
      if (i != 0)
         retVal += " ";

      int highlightVal = theBytesHighlighted.value(i, 0);

      QString dataString;
      if (theOptionsFlag & ASCII_DISPLAY)
      {
         // Display the ASCII characters
         dataString.append(readableAsciiConversion(static_cast<char>(theData.at(i))));
      }
      else
      {
         // Display the bytes as hexadecimal
         dataString = Helpers::qbyteToHexString(theData.mid(i, 1));
      }

      if (theOptionsFlag & SET_COLOR)
      {
          retVal += "<b class='forcemode'>";
      }
      else
      {
          retVal += "<b class='fdf";
          retVal += QString::number(highlightVal);
          retVal += "'>";
      }

      retVal += dataString;
      retVal += "</b>";
   }

   return retVal;
}

void FrameDataField::forceColor(QString colorString){
    theOptionsFlag |= SET_COLOR;
    theForceColor = colorString;

    theStyleString = "<style>\n";

    theStyleString += "  .forcemode";
    theStyleString += " { background-color: ";
    theStyleString += colorString;
    theStyleString += " } \n";

    theStyleString += "</style>";
}

void FrameDataField::unforceColor()
{
    theOptionsFlag &= ~ SET_COLOR;

    // Set the style string text back to normal
    setHighlightDuration(theHighlightInterval);
}

void FrameDataField::setAsciiFlag(bool displayAsAscii)
{
   if (displayAsAscii)
      theOptionsFlag |= ASCII_DISPLAY;
   else
      theOptionsFlag &= ~ ASCII_DISPLAY;
}

void FrameDataField::timerTick()
{
   fdfDebug() << __PRETTY_FUNCTION__;

   bool resetTimer = false;

   foreach(int byteNumber, theBytesHighlighted.keys())
   {
      int oldVal = theBytesHighlighted.value(byteNumber);

      if (oldVal == 1)
      {
         theBytesHighlighted.remove(byteNumber);
      }
      else
      {

         theBytesHighlighted[byteNumber] = --oldVal;
         resetTimer = true;
      }
   }

   if (theLabel != nullptr)
   {
      // fdfDebug() << "Update label on " << Helpers::qbyteToHexString(theData) << " to:"
      //            << getFieldValueRichString();
      theLabel->setText(getFieldValueRichString());
   }

   if (resetTimer)
   {
      fdfDebug() << __PRETTY_FUNCTION__ << " - restarting timer";
      theUpdateTimer.start(FRAMER_TIMER_TICK_DURATION_MS);
   }
}

QString FrameDataField::readableAsciiConversion(char data)
{
   QString retVal;
   if ( (data >= 20) && (data <= 0x7e))
   {
      if (data == 0x5c)
         return retVal.append("\\");

      return retVal.append(data);
   }

   switch(data)
   {
   case 0x09:
      retVal.append("\t");
      break;

   case 0x0a:
      retVal.append("\n");
      break;

   case 0x0c:
      retVal.append("\f");
      break;

   case 0x0d:
      retVal.append("\r");
      break;

   default:
      char smallBuf[128];
      sprintf(smallBuf, "\\x%02x", data);
      retVal.append(smallBuf);
   }

   return retVal;
}

int FrameDataField::colorIndexLookup(int ticksLeft)
{
   int indexesPerTick = NUM_SHADES / theHighlightInterval;

   int retIndex = ticksLeft * indexesPerTick;

   if (retIndex >= NUM_SHADES)
      retIndex = NUM_SHADES - 1;

   if (retIndex < 0)
      retIndex = 0;

   return retIndex;
}
