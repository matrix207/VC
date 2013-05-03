// winio.h

#ifndef __WINIO_H__
#define __WINIO_H__

int winio_init();
void winio_clean();
void outb(const unsigned char value, const unsigned char index);
void inb(unsigned char *pvalue, const unsigned char index);

#endif
