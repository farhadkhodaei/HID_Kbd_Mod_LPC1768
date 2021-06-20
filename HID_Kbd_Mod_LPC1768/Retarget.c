/*------------------------------------------------------------------------------
 *      RL-ARM
 *------------------------------------------------------------------------------
 *      Name:    Retarget.c
 *      Purpose: Retarget input to the USB keyboard and 
 *               output to the LCD display
 *------------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <RTL.h>
#include <stdio.h>
#include <string.h>
#include <rt_sys.h>
#include <rl_usb.h>
#include "GLCD.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;

#ifdef STDIO

/*------------------------------------------------------------------------------
  Write character to the LCD display
 *----------------------------------------------------------------------------*/
int sendchar (int ch) {
  static int lnd = 0;
  static int ln  = 0;
  static int col = 0;

  if (ch == 0x0A) {           /* Line Feed */
    col = 0;
    lnd++;
    ln ++;
    if (lnd >= 40) 
      lnd = 0;
    if (ln >= 40) {
      GLCD_ScrollVertical (8);
      GLCD_ClearLn (lnd, 0);
    }
  } else if (ch == 0x0D) {    /* Carriage Return */
    col = 0;
  } else if (ch == 0x08) {    /* Backspace */
    if (col) 
      col --;
  } else {
    GLCD_DisplayChar (lnd, col++, 0, (unsigned char) ch);
  }

  return (ch);
}


/*------------------------------------------------------------------------------
  Read character from the USB keyboard
 *----------------------------------------------------------------------------*/
int getkey (void) {

  return (usbh_hid_kbd_getkey (0, 0));
}
#endif

/*--------------------------- fputc ------------------------------------------*/

int fputc (int ch, FILE *f) {
#ifdef STDIO
  return (sendchar (ch));
#endif
}

/*--------------------------- fgetc ------------------------------------------*/

int fgetc (FILE *f) {
#ifdef STDIO
  return (getkey ());
#endif
}

/*--------------------------- ferror -----------------------------------------*/

int ferror (FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

/*--------------------------- _ttywrch ---------------------------------------*/

void _ttywrch (int ch) {
#ifdef STDIO
  sendchar (ch);
#endif
}

/*--------------------------- _sys_exit --------------------------------------*/

void _sys_exit (int return_code) {
  /* Endless loop. */
  while (1);
}

/*------------------------------------------------------------------------------
 * end of file
 *----------------------------------------------------------------------------*/
