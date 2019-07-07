
/*
 * INCLUDES
 */
#include "sercom.h"
#include "atmel_start.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*
 * VARIABLES
 */
struct io_descriptor *io;

/*
 * EXTERNAL FUNCTION DEFINITION
 */

void InitSercom(void)
{
#if (USE_RS232 == ON)
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);
#endif
}

void WriteToConsole(const char *s)
{
#if (USE_RS232 == ON)
	/* Write to UART (FTDI Board) */
	io_write(io, (const uint8_t *)s, strlen(s));
#else	
	/* Use MicroUSB EDBG Module */
	io_write(&EDBG_COM.io, (const uint8_t *)s, strlen(s));
#endif
}
