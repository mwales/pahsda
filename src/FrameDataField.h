#ifndef FRAMEDATAFIELD_H
#define FRAMEDATAFIELD_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QByteArray>

#include <QString>
#include <QTimer>

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

   void setHighlightDuration(int timerTicks);

   void updateValue(QByteArray data);

   void setValue(QByteArray data);

   QLabel* getLabel(QObject* parentParam);

   QByteArray getFieldRawValue();

   QString getFieldValueString();

   QString getFieldValueRichString();

   void forceColor(QString colorString);

   void unforceColor();

   void setAsciiFlag(bool displayAsAscii);

   enum
   {
      SET_COLOR = 1,
      ASCII_DISPLAY = 2
   };

protected slots:

   void timerTick();

protected:

   QString readableAsciiConversion(char data);

   int colorIndexLookup(int ticksLeft);

   QMap<int, int> theBytesHighlighted;

   QLabel* theLabel;

   int theHighlightInterval;

   QByteArray theData;

   QString theStyleString;

   // For when a field needs to be permenantly colored a color
   uint8_t theOptionsFlag;
   QString theForceColor;

   static const QStringList BACKGROUND_SHADES;

   static const int NUM_SHADES;

   QTimer theUpdateTimer;
};

#endif // FRAMEDATAFIELD_H
