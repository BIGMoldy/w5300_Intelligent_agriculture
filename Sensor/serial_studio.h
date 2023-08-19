#ifndef _SERIAL_STUDIO__H_
#define _SERIAL_STUDIO__H_
#include "stdint.h"

void Serial_Studio(void);
char uBinaryToAscii(unsigned int value, char *str);
char sBinaryToAscii(int value, char *str);
char binaryToAscii(int data, char *str);

#endif
