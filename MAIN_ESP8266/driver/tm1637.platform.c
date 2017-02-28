// Bondarenko Andrey <pt200>
// @mail
//
// I2C bit-banging driver

#include "ets_sys.h"
#include "osapi.h"

#include "driver/TM1637.platform.h"



#define SDA_PIN( i2c) ( ( i2c >> 8) & 0x0F)
#define SCL_PIN( i2c) ( i2c & 0x0F)
#define SDA_PIN_MASK( i2c) ( 1 << SDA_PIN( i2c))
#define SCL_PIN_MASK( i2c) ( 1 << SCL_PIN( i2c))

// Fucking esp with addresation
static unsigned int GPIO_MUX[ 16] =
{
PERIPHS_IO_MUX_GPIO0_U,
PERIPHS_IO_MUX_U0TXD_U,
PERIPHS_IO_MUX_GPIO2_U,
PERIPHS_IO_MUX_U0RXD_U,
PERIPHS_IO_MUX_GPIO4_U,
PERIPHS_IO_MUX_GPIO5_U,
0, 0, 0, 0, 0, 0,
PERIPHS_IO_MUX_MTDI_U,
PERIPHS_IO_MUX_MTCK_U,
PERIPHS_IO_MUX_MTMS_U,
PERIPHS_IO_MUX_MTDO_U
};
// ... and func
static unsigned int GPIO_FUNC[ 16] =
{
FUNC_GPIO0, FUNC_GPIO1, FUNC_GPIO2, FUNC_GPIO3,
FUNC_GPIO4, FUNC_GPIO5, 0, 0,
0, 0, 0, 0,
FUNC_GPIO12, FUNC_GPIO13, FUNC_GPIO14, FUNC_GPIO15
};
#define GPIO_BIT( n)  ( 1 << n)



static PLATFORM_CALL void _I2C_WAIT( PLATFORM_I2C_DRV i2c)
{
  os_delay_us( 5);
}
static PLATFORM_CALL void _I2C_SCL_0( PLATFORM_I2C_DRV i2c)
{
  _I2C_WAIT( i2c);
  gpio_output_set( 0, SCL_PIN_MASK( i2c), SCL_PIN_MASK( i2c), 0); // Direct push/pull drive
}
static PLATFORM_CALL void _I2C_SCL_1( PLATFORM_I2C_DRV i2c)
{
  _I2C_WAIT( i2c);
  gpio_output_set( SCL_PIN_MASK( i2c), 0, SCL_PIN_MASK( i2c), 0); // Direct push/pull drive
}
static PLATFORM_CALL void _I2C_SDA_0( PLATFORM_I2C_DRV i2c)
{
  _I2C_WAIT( i2c);
  gpio_output_set( 0, SDA_PIN_MASK( i2c), SDA_PIN_MASK( i2c), 0); // Direct pull drive
}
static PLATFORM_CALL void _I2C_SDA_1( PLATFORM_I2C_DRV i2c)
{
  _I2C_WAIT( i2c);
  gpio_output_set( 0, 0, 0, SDA_PIN_MASK( i2c)); // Pull Up
}
static PLATFORM_CALL int _I2C_GET_SDA( PLATFORM_I2C_DRV i2c)
{
  return ( gpio_input_get()& SDA_PIN_MASK( i2c));
}

PLATFORM_CALL int PLATFORM_I2C_DRV_init( PLATFORM_I2C_DRV i2c, int speed)
{
  PIN_FUNC_SELECT( GPIO_MUX[ SDA_PIN( i2c)], GPIO_FUNC[ SDA_PIN( i2c)]);
  PIN_FUNC_SELECT( GPIO_MUX[ SCL_PIN( i2c)], GPIO_FUNC[ SCL_PIN( i2c)]);

  _I2C_SDA_1( i2c);
  _I2C_SCL_1( i2c);

  return 0;
}

PLATFORM_CALL void PLATFORM_I2C_DRV_start( PLATFORM_I2C_DRV i2c)
{
  _I2C_SDA_1( i2c);
  _I2C_SCL_1( i2c);
  _I2C_SDA_0( i2c);
  _I2C_SCL_0( i2c);
}

PLATFORM_CALL void PLATFORM_I2C_DRV_stop( PLATFORM_I2C_DRV i2c)
{
  _I2C_SCL_0( i2c);
  _I2C_SDA_0( i2c);
  _I2C_SCL_1( i2c);
  _I2C_SDA_1( i2c);
}

PLATFORM_CALL int PLATFORM_I2C_DRV_send_byte( PLATFORM_I2C_DRV i2c, unsigned char v)
{
  unsigned char q;

  for( q = 0; q < 8; q++, v >>= 1)
  {
    if( v & 0x01)
      _I2C_SDA_1( i2c);
    else
      _I2C_SDA_0( i2c);

    _I2C_SCL_1( i2c);
    _I2C_SCL_0( i2c);
  }

//ACK !!!
  _I2C_SDA_1( i2c);
  _I2C_SCL_1( i2c);
  q = _I2C_GET_SDA( i2c);
  _I2C_SCL_0( i2c);

  return ( q == 0)?0:-1;
}
