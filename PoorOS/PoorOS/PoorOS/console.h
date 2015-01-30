//----------------------------------------------------------------------------

#ifndef POOROS_CONSOLE_H
#define POOROS_CONSOLE_H 1

#include "PoorOS/platform.h"

namespace PoorOS {

class System;

class Console
{
public:

  Console(void);

  virtual
  ~Console(void);

  virtual
  void
  clear(void);

  virtual
  void
  writeChar(char c);

  virtual
  void
  writeStr(const char * str);

  virtual
  void
  writeHexaByte(byte value);

  virtual
  void
  writeHexaWord(word value);

  virtual
  void
  writeHexaDWord(dword value);

  virtual
  void
  writeDecimalByte(byte value);

  virtual
  void
  writeDecimalWord(word value);

  virtual
  void
  writeDecimalDWord(dword value);

  virtual
  void
  writeFloat(float value);

  virtual
  void
  writeDouble(double value);

  virtual
  void
  beep(dword frequency,
       dword milliseconds);

  virtual
  void
  setCursor(dword column,
            dword row);

  virtual
  void
  getCursor(dword & columnOut,
            dword & rowOut) const;

  virtual
  size_t // nb columns
  getWidth(void) const;

  virtual
  size_t // nb rows
  getHeight(void) const;

  enum Color
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
    };
    
  virtual
  void
  setColor(Console::Color color);

  virtual
  Console::Color // current color
  getColor(void) const;

  virtual
  void
  setTabSize(size_t tabSize);

  virtual
  size_t // current tab size
  getTabSize(void) const;

  virtual
  size_t // nb chars in input buffer
  inputReady(void) const;

  virtual
  char // next char in input buffer or \0
  getChar(void);

  virtual
  bool // key is pressed
  testKey(byte key);

protected:

  friend class System;

  virtual
  void
  _manageScanCode(byte sc);

  virtual
  char
  _translateKey(byte key);

  enum
    {
    TXT_BASE=0x000B8000,
    TXT_WIDTH=80,
    TXT_HEIGHT=25,
    TXT_INPUT_SIZE=0x100
    };

  dword _column;
  dword _row;
  byte _color;
  byte _reverseColor;
  byte _savedColor;
  size_t _tabSize;
  volatile byte _keys[0x10];
  volatile char _input[Console::TXT_INPUT_SIZE];
  volatile size_t _inputReady;
};

} // namespace PoorOS

#endif // POOROS_CONSOLE_H

//----------------------------------------------------------------------------
