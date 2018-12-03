#include "BinaryFileIoDevice.h"
#include <QtDebug>

// Uncomment to enable debugging of this class
#define BINARY_FILE_DEBUG

#ifdef BINARY_FILE_DEBUG
   #define bfDebug qDebug
   #define bfWarning qWarning
#else
   #define bfDebug if(0) qDebug
   #define bfWarning if(0) qWarning
#endif

BinaryFileIoDevice::BinaryFileIoDevice(QString filename, QObject* parent):
   QFile(filename, parent),
   theTimer(parent),
   theFinishedTimer(parent),
   theTimeToReadFileMs(60000),
   theFileSize(0)
{
   bfDebug() << "BinaryFileIoDevice for file: " << filename;
   theTimer.setSingleShot(false);
   theTimer.setInterval(100);
   connect(&theTimer, &QTimer::timeout,
           this, &BinaryFileIoDevice::readyToReadTimer);

   theFinishedTimer.setSingleShot(true);
   connect(&theFinishedTimer, &QTimer::timeout,
           this, &BinaryFileIoDevice::readTimeComplete);

   open(QIODevice::ReadOnly);
}

qint64 BinaryFileIoDevice::bytesAvailable() const
{
   int readTimeSoFar = theTimeToReadFileMs - theFinishedTimer.remainingTime();
   readTimeSoFar = qBound(0, readTimeSoFar, theTimeToReadFileMs);

   qint64 filePosGivenTime = readTimeSoFar * theFileSize / theTimeToReadFileMs;

   qint64 bytesReadAlready = pos();
   qint64 retVal = qBound(0ll, (filePosGivenTime - bytesReadAlready), theFileSize);
   bfDebug() << __PRETTY_FUNCTION__ << " returning " << retVal << "bytes, pos =" << bytesReadAlready
             << ", time=" << readTimeSoFar;

   return retVal;
}

void BinaryFileIoDevice::setTimeToReadFile(int seconds)
{
   bfDebug() << __PRETTY_FUNCTION__ << seconds << "seconds";

   theTimeToReadFileMs = seconds * 1000;
}

void BinaryFileIoDevice::startReading()
{
   bfDebug() << __PRETTY_FUNCTION__;

   theFileSize = size();

   theTimer.start();
   theFinishedTimer.start(theTimeToReadFileMs);
}

void BinaryFileIoDevice::readTimeComplete()
{
   bfDebug() << __PRETTY_FUNCTION__;

   theTimer.stop();

   // Fire it one last time so the clients will read all the bytes of the file
   emit readyToReadTimer();
}
