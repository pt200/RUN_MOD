// Bondarenko Andrey <pt200>
// @mail

#include "osapi.h"
#include "user_interface.h"
//#include "espconn.h"
#include "mem.h"
//#include "math.h"

#include "driver/tm1637.h"
#include "driver/tm1637.platform.h"



const TM1637_SYMBOL _tm1637_digit_symbols[] =
  { S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, S_8, S_9};


struct _TM1637
{
#ifdef TM1637_USE_MAGIC
  int MAGIC;
#endif
  PLATFORM_I2C_DRV i2c;
  TM1637_SYMBOL vram[ TM1637_LEN];
};


void* tm1637_init( PLATFORM_I2C_DRV _i2c, unsigned char br)
{
  struct _TM1637* d = ( struct _TM1637*)os_zalloc( sizeof( struct _TM1637));
  if( d == NULL)
    return NULL;

  os_memset( d, 0, sizeof( struct _TM1637));

  d->i2c = _i2c;
#ifdef TM1637_USE_MAGIC
  d->MAGIC = TM1637_MAGIC;
#endif


  PLATFORM_I2C_DRV_init( d->i2c, 450);

  tm1637_update( ( void*)d);
  tm1637_bright( ( void*)d, br);

  return ( void*)d;
}

/*******************************************************************************
br = 0 - off
br = 100 - max
*******************************************************************************/
void tm1637_bright( void* d, unsigned char br)
{
  unsigned char b;
  if( d == NULL)
    return;

#ifdef TM1637_USE_MAGIC
  if( ( ( struct _TM1637*)d)->MAGIC != TM1637_MAGIC)
    return;
#endif

  if( br < 5) // OFF
    b = 0x80;
  else
  if( br >= 100) // MAX
    b = 0x8F;
  else
    b = 0x88 | ( ( br - 5) / 12);

  PLATFORM_I2C_DRV_start( ( ( struct _TM1637*)d)->i2c);
  PLATFORM_I2C_DRV_send_byte( ( ( struct _TM1637*)d)->i2c, b);
  PLATFORM_I2C_DRV_stop( ( ( struct _TM1637*)d)->i2c);
}

/*******************************************************************************
*******************************************************************************/
void tm1637_update( void* d)
{
  unsigned char b;
  int q;

  if( d == NULL)
    return;

#ifdef TM1637_USE_MAGIC
  if( ( ( struct _TM1637*)d)->MAGIC != TM1637_MAGIC)
    return;
#endif
  PLATFORM_I2C_DRV_start( ( ( struct _TM1637*)d)->i2c);

  b = 0x40; // Write + addr auto inc
  PLATFORM_I2C_DRV_send_byte( ( ( struct _TM1637*)d)->i2c, b);

  PLATFORM_I2C_DRV_stop( ( ( struct _TM1637*)d)->i2c);




  PLATFORM_I2C_DRV_start( ( ( struct _TM1637*)d)->i2c);

  b = 0xC0; // addr = 0
  PLATFORM_I2C_DRV_send_byte( ( ( struct _TM1637*)d)->i2c, b);

  for( q = 0; q < TM1637_LEN; q++)
  {
    b = ( ( struct _TM1637*)d)->vram[ q];
    PLATFORM_I2C_DRV_send_byte( ( ( struct _TM1637*)d)->i2c, b);
  }

  PLATFORM_I2C_DRV_stop( ( ( struct _TM1637*)d)->i2c);
}

/*******************************************************************************
*******************************************************************************/
void tm1637_set_vram_7seg( void* d, unsigned int pos, TM1637_SYMBOL s)
{
  if( d == NULL)
    return;

#ifdef TM1637_USE_MAGIC
  if( ( ( struct _TM1637*)d)->MAGIC != TM1637_MAGIC)
    return;
#endif

  if( pos < TM1637_LEN)
    ( ( struct _TM1637*)d)->vram[ pos] = s;
}

/*******************************************************************************
*******************************************************************************/
/*
void tm1637_set_vram_segs_all( void* d, unsigned int pos, unsigned char s, int len)
{
}
*/

/*******************************************************************************
*******************************************************************************/
TM1637_SYMBOL _tm1637_symbol_to_7seg( char c)
{
  if( ( c >= '0') && ( c <= '9'))
    return _tm1637_digit_symbols[ c - '0']; // '0' - '9'
  else
  if( c == '-')
    return S_MINUS; // '-'
  else
    return S_SPACE; // ' ' SPACE
}

/*******************************************************************************
*******************************************************************************/
void tm1637_set_vram_symbol( void* d, unsigned int pos, char c)
{
  tm1637_set_vram_7seg( d, pos, _tm1637_symbol_to_7seg( c));
}

/*******************************************************************************
*******************************************************************************/
/*
void tm1637_set_vram_symbols( void* d, unsigned int pos, char* c, int len)
{
}
*/

/*******************************************************************************
*******************************************************************************/
void tm1637_show_int( void* d, int v)
{
  int pos;
//  if( d == NULL)
//    return;
  if( v > 9999)
  {
    tm1637_set_vram_7seg( d, 0, S_H);
    tm1637_set_vram_7seg( d, 1, S_H);
    tm1637_set_vram_7seg( d, 2, S_H);
    tm1637_set_vram_7seg( d, 3, S_H);
  }
  else
  if( v < -999)
  {
    tm1637_set_vram_7seg( d, 0, S_L);
    tm1637_set_vram_7seg( d, 1, S_L);
    tm1637_set_vram_7seg( d, 2, S_L);
    tm1637_set_vram_7seg( d, 3, S_L);
  }
  else
  {
    tm1637_set_vram_7seg( d, 3, _tm1637_digit_symbols[ abs( v % 10)]);
    for( pos = 2; pos > -1; pos--)
    {
      if( ( v >= 10) || ( v <= -10))
      {
        v /= 10;
        tm1637_set_vram_7seg( d, pos, _tm1637_digit_symbols[ abs( v % 10)]);
      }
      else
      if( v < 0)
      {
        v = 0;
        tm1637_set_vram_7seg( d, pos, S_MINUS);
      }
      else
        tm1637_set_vram_7seg( d, pos, S_SPACE);
    }
  }

  tm1637_update( d);
}

/*******************************************************************************
*******************************************************************************/
/*
void tm1637_show_uint( void* d, unsigned int v)
{
  int pos;
  if( v > 9999)
  {
    tm1637_set_vram( d, 0, S_H);
    tm1637_set_vram( d, 1, S_H);
    tm1637_set_vram( d, 2, S_H);
    tm1637_set_vram( d, 3, S_H);
  }
  else
  {
    tm1637_set_vram( d, 3, _tm1637_digit_symbols[ v % 10]);
    for( pos = 2; pos; pos--)
    {
      if( v >= 10)
      {
        v /= 10;
        tm1637_set_vram( d, pos, _tm1637_digit_symbols[ v % 10]);
      }
      else
        tm1637_set_vram( d, pos, S_SPACE);
    }
  }

  tm1637_update( d);
}
*/
