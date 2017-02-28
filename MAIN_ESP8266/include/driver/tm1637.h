// Bondarenko Andrey <pt200>
// @mail
#include "driver/tm1637.platform.h"

#ifndef __TM1637_H__
#define __TM1637_H__

//#define TM1637_USE_MAGIC

//#define NULL  ( ( void*)0)


typedef unsigned char TM1637_SYMBOL;


#define TM1637_LEN	4
#define TM1637_MAGIC	0x01ed


#define TM1637_SEG_A	0x01
#define TM1637_SEG_B	0x02
#define TM1637_SEG_C	0x04
#define TM1637_SEG_D	0x08
#define TM1637_SEG_E	0x10
#define TM1637_SEG_F	0x20
#define TM1637_SEG_G	0x40
#define TM1637_SEG_DP	0x80


#define S_0	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F)
#define S_1	( TM1637_SEG_B | TM1637_SEG_C)
#define S_2	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_G)
#define S_3	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_G)
#define S_4	( TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_F | TM1637_SEG_G)
#define S_5	( TM1637_SEG_A | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_F | TM1637_SEG_G)
#define S_6	( TM1637_SEG_A | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G)
#define S_7	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C)
#define S_8	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G)
#define S_9	( TM1637_SEG_A | TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_D | TM1637_SEG_F | TM1637_SEG_G)

#define S_SPACE	( 0)
#define S_MINUS	( TM1637_SEG_G)
#define S_L	( TM1637_SEG_D | TM1637_SEG_E | TM1637_SEG_F)
#define S_H	( TM1637_SEG_B | TM1637_SEG_C | TM1637_SEG_E | TM1637_SEG_F | TM1637_SEG_G)




#define DIGIT_0	0
#define DIGIT_1	1
#define DIGIT_2	2
#define DIGIT_3	3



void* tm1637_init( PLATFORM_I2C_DRV _I2c, unsigned char br);
void tm1637_bright( void* d, unsigned char br);
void tm1637_update( void* d);
void tm1637_set_vram_7seg( void* d, unsigned int pos, TM1637_SYMBOL s);
void tm1637_set_vram_symbol( void* d, unsigned int pos, char c);
void tm1637_show_int( void* d, int v);



#endif
