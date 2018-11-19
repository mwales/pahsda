#include "DataFrame.h"

#include <QtDebug>

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
   dFrameDebug() << "Created a new data frame";
}

int DataFrame::getNumFields()
{
   return theFields.size();
}

QList<int> DataFrame::getFieldIndexes()
{
   return theFields.keys();
}

QString DataFrame::getFieldName(int fieldIndex)
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

QString DataFrame::getFieldAbbrev(int field)
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

QByteArray DataFrame::getFieldRawValue(int fieldIndex)
{
   if (theValues.contains(fieldIndex))
   {
      return theValues.value(fieldIndex);
   }
   else
   {
      dFrameWarning() << "Asked for data from invalid fieldIndex = " << fieldIndex;
      return QByteArray();
   }
}

QString DataFrame::getFieldValueString(int fieldIndex)
{
   if (theValues.contains(fieldIndex))
   {
      return theValues.value(fieldIndex).toHex(' ');
   }
   else
   {
      dFrameWarning() << "Asked for data from invalid fieldIndex = " << fieldIndex;
      return "";
   }
}

QString DataFrame::getFieldValueRichString(int fieldIndex)
{
   return getFieldValueString(fieldIndex);
}

QDateTime DataFrame::getTimestamp()
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

bool DataFrame::addValue(int fieldIndex, QByteArray rawBytes)
{
   if (theFields.contains(fieldIndex))
   {
      theValues[fieldIndex] = rawBytes;
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

bool DataFrame::operator<(const DataFrame& rhs) const
{
   dFrameDebug() << "DataFrame < DataFrame called";

   foreach(int sortingIndex, theSortingIndexes)
   {

      // Make sure that both frames have this index
      if ( !theFields.contains(sortingIndex) ||
           !rhs.theFields.contains(sortingIndex) )
      {
         qFatal("Comparing two frames that don't have the sorting index");
      }

      QByteArray lhsBytes = theValues.value(sortingIndex);
      QByteArray rhsBytes = rhs.theValues.value(sortingIndex);

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
            return lhsBytes[i] < rhsBytes[i];
         }
      }
   }

   // We went though all the sorting indexes, and haven't found a differentce, they are equal
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
   dFrameDebug() << "DataFrame == DataFrame called";

   foreach(int sortingIndex, theSortingIndexes)
   {

      // Make sure that both frames have this index
      if ( !theFields.contains(sortingIndex) ||
           !rhs.theFields.contains(sortingIndex) )
      {
         qFatal("Comparing two frames that don't have the sorting index");
      }

      QByteArray lhsBytes = theValues.value(sortingIndex);
      QByteArray rhsBytes = rhs.theValues.value(sortingIndex);

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
