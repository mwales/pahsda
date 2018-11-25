#ifndef FRAMEDATAFIELD_H
#define FRAMEDATAFIELD_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QByteArray>

#include <QString>

class QLabel;

/**
 * Single field of a data frame.  It controls a QLabel that we will display on the main GUI.
 *
 * The QLabel can display rich text (so we can color / format the label text)
 * We can have timers and things, so we can automatically update the label as well
 *
 * @todo Track changes to the raw bytes, highlight the changes in the rich text view
 */
class FrameDataField: public QObject
{

Q_OBJECT

public:
   FrameDataField(QObject* parent = Q_NULLPTR);

   void setHighlightDuration(int millisecs);

   void updateValue(QByteArray data);

   void setValue(QByteArray data);

   QLabel* getLabel();

   QByteArray getFieldRawValue();

   QString getFieldValueString();

   QString getFieldValueRichString();

protected:

   QTimer theColorChangeTimerTick;

   QMap<int, int> theBytesHighlighted;

   QLabel* theLabel;

   int theHighlightInterval;

   QByteArray theData;
};

#endif // FRAMEDATAFIELD_H
