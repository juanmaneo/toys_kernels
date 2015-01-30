/*--------------------------------------------------------------------------*/

#ifndef TXTSCREEN_H
#define TXTSCREEN_H

#include "platform.h"

typedef  enum
  {
  TXT_BLUE=0x01,
  TXT_GREEN=0x02,
  TXT_CYAN=0x03,
  TXT_RED=0x04,
  TXT_MAGENTA=0x05,
  TXT_BROWN=0x06,
  TXT_LIGHT_GREY=0x07,
  TXT_GREY=0x08,
  TXT_LIGHT_BLUE=0x09,
  TXT_LIGHT_GREEN=0x0A,
  TXT_LIGHT_CYAN=0x0B,
  TXT_LIGHT_RED=0x0C,
  TXT_PINK=0x0D,
  TXT_YELLOW=0x0E,
  TXT_WHITE=0x0F
  } TxtColor;

void
setTxtColor(TxtColor color);

TxtColor
getTxtColor(void);

void
setTxtTabSize(size_t tabSize);

size_t
getTxtTabSize(void);

void
setTxtPos(dword column,
          dword row);

void
getTxtPos(dword * columnOut,
          dword * rowOut);

size_t
getTxtWidth(void);

size_t
getTxtHeight(void);

void
clearScreen(void);

void
writeChar(char c);

void
writeStr(const char * str);

void
writeHexaByte(byte value);

void
writeHexaWord(word value);

void
writeHexaDWord(dword value);

#define writeDecimalByte(value) \
        writeDecimalDWord(value)

#define writeDecimalWord(value)  \
        writeDecimalDWord(value)

void
writeDecimalDWord(dword value);

void
writeFloat(float value);

void
writeDouble(double value);

void
beep(dword frequency,
     dword milliseconds);

#endif /* TXTSCREEN_H */

/*--------------------------------------------------------------------------*/
