#ifndef DUMLFRAME_H
#define DUMLFRAME_H

#include "DataFrame.h"

/**
 * Will divide the DUML framed data into different fields for easy viewing and sorting
 */
class DumlFrame : public DataFrame
{
public:
   DumlFrame(QByteArray framedData);
};

#endif // DUMLFRAME_H
