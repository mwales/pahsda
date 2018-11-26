#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QVector>


class FrameDataField;
class QLabel;
class QObject;

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
   ~DataFrame();

   int getNumFields();

   QList<int> getFieldIndexes() const;

   QString getFieldName(int field) const;

   QString getFieldAbbrev(int field) const;

   QByteArray getFieldRawValue(int fieldIndex) const;

   QString getFieldValueString(int fieldIndex) const;

   QString getFieldValueRichString(int fieldIndex) const;

   QDateTime getTimestamp() const;

   bool addField(int fieldIndex, QString fieldName, QString abbrev);

   bool updateFieldValue(int fieldIndex, QByteArray rawBytes);

   void setSortingIndexes(QList<int> sortOrdering);

   void updateFrame(DataFrame const & df);

   // Sorting operations
   bool operator<(const DataFrame& rhs) const;
   bool operator<=(const DataFrame& rhs) const;
   bool operator==(const DataFrame& rhs) const;
   bool operator!=(const DataFrame& rhs) const;
   bool operator>=(const DataFrame& rhs) const;
   bool operator>(const DataFrame& rhs) const;

   QString toString() const;

   QLabel* getLabel(int fieldIndex, QObject* parent);

   void setHighlighting(QVector<int> indexList, int numTicks);

protected:

   /// @todo Used shared pointers for the following field so we don't waste time / memory
   QMap<int, QString> theFields;

   /// @todo Used share pointer for the following field so we don't waste time / memory
   QMap<int, QString> theAbbreviations;

   QVector<int> theHighlightedFields;

   int theHighlightDuration;

   QDateTime theRxTime;

   QMap<int, FrameDataField*> theValues;

   /**
    * The messages will be sorted by these indexes, in the order of the first in the list is used
    * first in sorting
    */
   QList<int> theSortingIndexes;



};

#endif // DATAFRAME_H
