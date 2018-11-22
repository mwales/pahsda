#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QList>
#include <QByteArray>

/**
 * Base class for all frame types.  Each frame should be divided into parts (fields).  Field name
 * should describe what that part of the frame means.  Index the frames in the order you want them
 * to appear the user.  The sorting indexes are used to determine how the frames are sorted (only
 * the indexes you specify will be used for sorting the frames)
 */
class DataFrame
{
public:
   DataFrame();

   int getNumFields();

   QList<int> getFieldIndexes();

   QString getFieldName(int field);

   QString getFieldAbbrev(int field);

   QByteArray getFieldRawValue(int fieldIndex);

   QString getFieldValueString(int fieldIndex);

   QString getFieldValueRichString(int fieldIndex);

   QDateTime getTimestamp();

   bool addField(int fieldIndex, QString fieldName, QString abbrev);

   bool addValue(int fieldIndex, QByteArray rawBytes);

   void setSortingIndexes(QList<int> sortOrdering);

   // Sorting operations
   bool operator<(const DataFrame& rhs) const;
   bool operator<=(const DataFrame& rhs) const;
   bool operator==(const DataFrame& rhs) const;
   bool operator!=(const DataFrame& rhs) const;
   bool operator>=(const DataFrame& rhs) const;
   bool operator>(const DataFrame& rhs) const;

   QString toString() const;

protected:

   /// @todo Used shared pointers for the following field so we don't waste time / memory
   QMap<int, QString> theFields;

   /// @todo Used share pointer for the following field so we don't waste time / memory
   QMap<int, QString> theAbbreviations;

   QDateTime theRxTime;

   QMap<int, QByteArray> theValues;

   /**
    * The messages will be sorted by these indexes, in the order of the first in the list is used
    * first in sorting
    */
   QList<int> theSortingIndexes;

};

#endif // DATAFRAME_H
