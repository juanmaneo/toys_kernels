/*--------------------------------------------------------------------------*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "platform.h"

size_t /* nb chars in input buffer */
inputReady(void);

char /* next char in input buffer or \0 */
getChar(void);

bool /* key is pressed */
testKey(byte key);

void
keyboardInterruptHandler(void);

#endif /* KEYBOARD_H */

/*--------------------------------------------------------------------------*/
