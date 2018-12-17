#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QByteArray>

class Helpers
{
public:

    static QString qbyteToHexString(QByteArray data);

    static uint32_t qbytearrayToValue(QByteArray data);

};

#endif // HELPERS_H
