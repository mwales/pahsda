#include "Helpers.h"

QString Helpers::qbyteToHexString(QByteArray data)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
    return data.toHex(' ');
#else
    QString retVal;

    for(int i = 0; i < data.length(); i++)
    {
        if (i != 0)
            retVal += " ";

        retVal += data.mid(i, 1).toHex();
    }

    return retVal;
#endif
}
