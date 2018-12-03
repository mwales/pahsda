#ifndef BINARYIODEVICE_H
#define BINARYIODEVICE_H

#include <QObject>
#include <QFile>
#include <QTimer>

/**
 * This file will allow a binary file that already exists to be slowly read into the system over
 * time.  Normally a Qt application could just read an entire binary file at once, but that would
 * not give a human time to see the messages come in over time.
 *
 * @note Implment the reader by calling bytesAvailabe() followed by read(), specifying the number
 * of bytes available returned from the previous call.  readAll() isn't virtual, and will still
 * return the entire file contents instantly.
 */
class BinaryFileIoDevice : public QFile
{
   Q_OBJECT

public:
   BinaryFileIoDevice(QString filename, QObject* parent = Q_NULLPTR);

   virtual qint64 bytesAvailable() const;

   void setTimeToReadFile(int seconds);

   void startReading();

signals:

   void readyToReadTimer();

protected slots:

   void readTimeComplete();

protected:

   // Timer that goes off every 100ms to cause users to read a little bit more of the file
   QTimer theTimer;

   // This timer is dual purpose.  It keeps track of how long the entire read is, and it also
   // tells when to turn off theTimer from repeatedly firing
   QTimer theFinishedTimer;

   int theTimeToReadFileMs;

   qint64 theFileSize;
};

#endif // BEAGLEIODEVICE_H
