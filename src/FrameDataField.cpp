#include "FrameDataField.h"

#include <QLabel>

#include "Helpers.h"


FrameDataField::FrameDataField(QObject* parent):
   QObject(parent),
   theLabel(nullptr),
   theHighlightInterval(0)
{

}

void FrameDataField::setHighlightDuration(int millisecs)
{
   theHighlightInterval = millisecs;
}

void FrameDataField::updateValue(QByteArray data)
{
   theData = data;

   if (theLabel != nullptr)
   {
      theLabel->setText(Helpers::qbyteToHexString(theData));
   }
}

void FrameDataField::setValue(QByteArray data)
{
   theData = data;
}

QLabel* FrameDataField::getLabel()
{
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
   return Helpers::qbyteToHexString(theData);
}
