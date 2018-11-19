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

   const int DUML_SOF = 0;
   const int DUML_FRAME_LEN = 1;
   const int DUML_FRAME_HEADER_CRC8 = 2;
   const int DUML_SOURCE = 3;
   const int DUML_TARGET = 4;
   const int DUML_SEQUENCE = 5;
   const int DUML_FLAGS = 6;
   const int DUML_COMMAND_SET = 7;
   const int DUML_COMMAND_ID = 8;
   const int DUML_PAYLOAD = 9;
   const int DUML_FRAME_CRC16 = 10;

};

#endif // DUMLFRAME_H
