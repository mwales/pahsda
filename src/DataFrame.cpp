#include "DataFrame.h"

#include <QtDebug>

#include "Helpers.h"
#include "FrameDataField.h"

// Uncomment to enable debugging of this class
#define DATA_FRAME_DEBUG

#ifdef DATA_FRAME_DEBUG
   #define dFrameDebug qDebug
   #define dFrameWarning qWarning
#else
   #define dFrameDebug if(0) qDebug
   #define dFrameWarning if(0) qWarning
#endif

DataFrame::DataFrame():
   theRxTime(QDateTime::currentDateTime())
{
   // dFrameDebug() << "Created a new data frame";
}

DataFrame::~DataFrame()
{
   foreach(FrameDataField* fd, theValues)
   {
      delete fd;
   }
}

int DataFrame::getNumFields()
{
   return theFields.size();
}

QList<int> DataFrame::getFieldIndexes() const
{
   return theFields.keys();
}

QString DataFrame::getFieldName(int fieldIndex) const
{
   if (theFields.contains(fieldIndex))
   {
      return theFields[fieldIndex];
   }
   else
   {
      return "";
   }
}

QString DataFrame::getFieldAbbrev(int field) const
{
   if (theAbbreviations.contains(field))
   {
      return theAbbreviations[field];
   }
   else
   {
      return "";
   }
}

QByteArray DataFrame::getFieldRawValue(int fieldIndex) const
{
   if (theValues.contains(fieldIndex))
   {
      return theValues.value(fieldIndex)->getFieldRawValue();
   }
   else
   {
      dFrameWarning() << "Asked for data from invalid fieldIndex = " << fieldIndex;
      return QByteArray();
   }
}

QString DataFrame::getFieldValueString(int fieldIndex) const
{
   if (theValues.contains(fieldIndex))
   {
      return theValues.value(fieldIndex)->getFieldValueString();
   }
   else
   {
      dFrameWarning() << "Asked for data from invalid fieldIndex = " << fieldIndex;
      return "";
   }
}

QString DataFrame::getFieldValueRichString(int fieldIndex) const
{
   if (theValues.contains(fieldIndex))
   {
      return theValues.value(fieldIndex)->getFieldValueRichString();
   }
   else
   {
      dFrameWarning() << "Asked for rich data string from invalid fieldIndex = " << fieldIndex;
      return "";
   }
}

QDateTime DataFrame::getTimestamp() const
{
   return theRxTime;
}

bool DataFrame::addField(int fieldIndex, QString fieldName, QString abbrev)
{
   if (theFields.contains(fieldIndex))
   {
      dFrameWarning() << "Trying to add field [" << fieldIndex << "," << fieldName << "] to dataframe, "
                      << "but that field index already exists (" << theFields.value(fieldIndex) << ")";
      return false;
   }

    theFields.insert(fieldIndex, fieldName);

    // We will assume that if we were OK to add the field, there isn't an existing abbreviation
    theAbbreviations.insert(fieldIndex, abbrev);

    return true;
}

bool DataFrame::updateFieldValue(int fieldIndex, QByteArray rawBytes)
{
   if (theFields.contains(fieldIndex))
   {
      // Is this the first time this field's value has been set?
      if (!theValues.contains(fieldIndex))
      {
         FrameDataField* fdf = new FrameDataField();
         fdf->setValue(rawBytes);

         theValues[fieldIndex] = fdf;
      }
      else
      {
         // A FrameDataField object already exists, just update it
         theValues.value(fieldIndex)->updateValue(rawBytes);
      }

      return true;
   }
   else
   {
      dFrameWarning() << "Tring to add a value to frame using invalid index " << fieldIndex;
      return false;
   }
}

void DataFrame::setSortingIndexes(QList<int> sortOrdering)
{
   foreach(int sortIndex, sortOrdering)
   {
      if (!theFields.contains(sortIndex))
      {
         dFrameWarning() << "You specified an invalid sorting index " << sortIndex;
         return;
      }
   }

   theSortingIndexes = sortOrdering;
}

void DataFrame::updateFrame(DataFrame const & df)
{
   foreach(int fieldIndex, theValues.keys())
   {
      theValues.value(fieldIndex)->updateValue(df.getFieldRawValue(fieldIndex));
   }
}

bool DataFrame::operator<(const DataFrame& rhs) const
{
   // dFrameDebug() << "DataFrame < DataFrame called";
   // dFrameDebug() << "  LHS: " << toString();
   // dFrameDebug() << "  RHS: " << rhs.toString();

   foreach(int sortingIndex, theSortingIndexes)
   {

      // Make sure that both frames have this index
      if ( !theFields.contains(sortingIndex) ||
           !rhs.theFields.contains(sortingIndex) )
      {
         qFatal("Comparing two frames that don't have the sorting index");
      }

      QByteArray lhsBytes = theValues.value(sortingIndex)->getFieldRawValue();
      QByteArray rhsBytes = rhs.theValues.value(sortingIndex)->getFieldRawValue();

      int numBytesToSort = lhsBytes.length();
      if (numBytesToSort != rhsBytes.length())
      {
         dFrameWarning() << "Sorting frames, and frame index " << sortingIndex
                         << "is different lengths in each of the 2 frames";

         if (numBytesToSort > rhsBytes.length())
         {
            numBytesToSort  = rhsBytes.length();
         }
      }

      for(int i = 0; i < numBytesToSort; i++)
      {
         if (lhsBytes[i] != rhsBytes [i])
         {
            // dFrameDebug() << "  " << (lhsBytes[i] < rhsBytes[i] ? "True" : "False");
            return lhsBytes[i] < rhsBytes[i];
         }
      }
   }

   // We went though all the sorting indexes, and haven't found a difference, they are equal
   return false;
}

bool DataFrame::operator<=(const DataFrame& rhs) const
{
   if (*this == rhs)
   {
      return true;
   }

    return (*this < rhs);
}

bool DataFrame::operator==(const DataFrame& rhs) const
{
   // dFrameDebug() << "DataFrame == DataFrame called";
   // dFrameDebug() << "  LHS: " << toString();
   // dFrameDebug() << "  RHS: " << rhs.toString();

   foreach(int sortingIndex, theSortingIndexes)
   {
      // Make sure that both frames have this index
      if ( !theFields.contains(sortingIndex) ||
           !rhs.theFields.contains(sortingIndex) )
      {
         qFatal("Comparing two frames that don't have the sorting index");
      }

      QByteArray lhsBytes = theValues.value(sortingIndex)->getFieldRawValue();
      QByteArray rhsBytes = rhs.theValues.value(sortingIndex)->getFieldRawValue();

      int numBytesToSort = lhsBytes.length();
      if (numBytesToSort != rhsBytes.length())
      {
         dFrameWarning() << "Sorting frames, and frame index " << sortingIndex
                         << "is different lengths in each of the 2 frames";

         if (numBytesToSort > rhsBytes.length())
         {
            numBytesToSort = rhsBytes.length();
         }
      }

      for(int i = 0; i < numBytesToSort; i++)
      {
         if (lhsBytes[i] != rhsBytes [i])
         {
            return false;
         }
      }
   }

   // We went though all the sorting indexes, and haven't found a differentce, they are equal
   return true;
}

bool DataFrame::operator!=(const DataFrame& rhs) const
{
   return !(*this == rhs);
}

bool DataFrame::operator>=(const DataFrame& rhs) const
{
   return !(*this < rhs);
}

bool DataFrame::operator>(const DataFrame& rhs) const
{
   if (*this == rhs)
   {
      return false;
   }

   return !(*this < rhs);
}

QString DataFrame::toString() const
{
   QString retVal;
   retVal += "Fields: ";
   foreach(int fieldIndex, theFields.keys())
   {
      retVal += "  ";
      retVal += QString::number(fieldIndex);
      retVal += " = ";
      retVal += theFields.value(fieldIndex);
      retVal += " = ";
      retVal += theValues.value(fieldIndex)->getFieldValueString();
      retVal += ", ";
   }
   return retVal;


}

QLabel* DataFrame::getLabel(int fieldIndex, QObject* parent)
{
   if (!theFields.contains(fieldIndex))
   {
      dFrameWarning() << "Trying to get label for non-existant field " << fieldIndex;
      return nullptr;
   }

   return theValues.value(fieldIndex)->getLabel(parent);
}

void DataFrame::setHighlighting(QVector<int> indexList, int numTicks)
{
   theHighlightedFields.clear();

   foreach(int fieldIndex, indexList)
   {
      if (theFields.contains(fieldIndex))
      {
         theHighlightedFields.push_back(fieldIndex);
      }
      else
      {
         dFrameWarning() << "Trying to highlight an invalid index: " << fieldIndex;
         continue;
      }

      if (theValues.contains(fieldIndex))
      {
         // A frame was already created
         theValues.value(fieldIndex)->setHighlightDuration(numTicks);
      }
   }

   theHighlightDuration = numTicks;
}

void DataFrame::colorField(int fieldIndex, QString color)
{
    if (theValues.contains(fieldIndex))
    {
       theValues.value(fieldIndex)->forceColor(color);
    }
}
