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
      theData = data;

      if (theLabel != nullptr)
      {
         theLabel->setText(getFieldValueRichString());
      }

      return;
   }

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
      theLabel = new QLabel();
      theLabel->setTextFormat(Qt::RichText);
      theLabel->setText(Helpers::qbyteToHexString(theData));
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
   return Helpers::qbyteToHexString(theData);
}

QString FrameDataField::getFieldValueRichString()
{
   QString retVal = theStyleString;

   for(int i = 0; i < theData.length(); i++)
   {
      if (i != 0)
         retVal += " ";

      int highlightVal = theBytesHighlighted.value(i, 0);

      if (theForceColorFlag)
      {
          retVal += "<b class='forcemode'>";
      }
      else
      {
          retVal += "<b class='fdf";
          retVal += QString::number(highlightVal);
          retVal += "'>";
      }

      retVal += Helpers::qbyteToHexString(theData.mid(i, 1));
      retVal += "</b>";
   }

   return retVal;
}

void FrameDataField::forceColor(QString colorString){
    theForceColorFlag = true;
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
    theForceColorFlag = false;

    // Set the style string text back to normal
    setHighlightDuration(theHighlightInterval);
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
