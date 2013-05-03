#include <stdio.h>
#include "watchdog.h"
#include "winio.h"

/* I/O Port's */
#define SM_REGINDEX	0x2e     /* SuperMicro ICH4+ Register Index */
#define SM_DATAIO	0x2f     /* SuperMicro ICH4+ Register Data I/O */

/* Control Register's */
#define SM_CTLPAGESW 0x07    /* SuperMicro ICH4+ Control Page Switch */
#define SM_CTLPAGE   0x08    /* SuperMicro ICH4+ Control Page Num */

#define SM_WATCHENABLE 0x30  /* Watchdog enable: Bit 0: 0=off, 1=on */

#define SM_WATCHPAGE 0x87    /* Watchdog unlock control page */

#define SM_ENDWATCH 0xAA     /* Watchdog lock control page */

#define SM_COUNTMODE 0xf5    /* Watchdog count mode select */
                             /* (Bit 3: 0 = seconds, 1 = minutes */

#define SM_WATCHTIMER 0xf6   /* 8-bits, Watchdog timer counter (RW) */

#define SM_RESETCONTROL	0xf7 /* Watchdog reset control */
                             /* Bit 6: timer is reset by kbd interrupt */
                             /* Bit 7: timer is reset by mouse interrupt */

static void supermicro_new_unlock_watchdog(void)
{
	/* Write 0x87 to port 0x2e twice */
	outb(SM_WATCHPAGE, SM_REGINDEX);
	outb(SM_WATCHPAGE, SM_REGINDEX);
	/* Switch to watchdog control page */
	outb(SM_CTLPAGESW, SM_REGINDEX);
	outb(SM_CTLPAGE, SM_DATAIO);
}

static void supermicro_new_lock_watchdog(void)
{
	outb(SM_ENDWATCH, SM_REGINDEX);
}

static unsigned char supermicro_get_byte_value(const unsigned char index)
{
	unsigned char value = 0;
	outb(index, SM_REGINDEX);
	inb(&value, SM_DATAIO);

	return value;
}

void supermicro_watchdog_status()
{
	unsigned char value = 0;

	supermicro_new_unlock_watchdog();

	value = supermicro_get_byte_value(SM_WATCHENABLE);
	value = value & 0x1;
	printf("watchdog is %s.\n", (value==0)?"disable":"enable");

	value = supermicro_get_byte_value(SM_COUNTMODE);
	unsigned char unit = value & 0x8;

	value = supermicro_get_byte_value(SM_WATCHTIMER);
	printf("Watch timer is %d %s\n", value, (unit==0)?"second":"minute");

	value = supermicro_get_byte_value(SM_RESETCONTROL);
	printf("Watchdog timer is %s can be interrupt by mouse\n", ((value & 0x80)==0)?"not":"");
	printf("Watchdog timer is %s can be interrupt by keyboard\n\n", ((value & 0x40)==0)?"not":"");

	supermicro_new_lock_watchdog();
}

void supermicro_keepalive(unsigned int heartbeat)
{
	supermicro_new_unlock_watchdog();

	/* reset watchdog timeout to heartbeat value */
	outb(SM_WATCHTIMER, SM_REGINDEX);
	outb((heartbeat & 255), SM_DATAIO);

	supermicro_new_lock_watchdog();
}