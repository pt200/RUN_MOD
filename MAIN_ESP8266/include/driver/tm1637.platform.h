// Bondarenko Andrey <pt200>
// @mail
#include "ets_sys.h"
#include "osapi.h"

#ifndef __TM1637_PLATFORM_H__
#define __TM1637_PLATFORM_H__

typedef uint32_t PLATFORM_I2C_DRV;

#define GPIO_PIN0	0
#define GPIO_PIN1	1
#define GPIO_PIN2	2
#define GPIO_PIN3	3
#define GPIO_PIN4	4
#define GPIO_PIN5	5
#define GPIO_PIN12	12
#define GPIO_PIN13	13
#define GPIO_PIN14	14
#define GPIO_PIN15	15

#define I2C_DRV( SDA_PIN, SCL_PIN)	( ( SDA_PIN << 8) | SCL_PIN)


#define PLATFORM_CALL	ICACHE_FLASH_ATTR


PLATFORM_CALL int PLATFORM_I2C_DRV_init( PLATFORM_I2C_DRV i2c, int speed);
PLATFORM_CALL void PLATFORM_I2C_DRV_start( PLATFORM_I2C_DRV i2c);
PLATFORM_CALL void PLATFORM_I2C_DRV_stop( PLATFORM_I2C_DRV i2c);
PLATFORM_CALL int PLATFORM_I2C_DRV_send_byte( PLATFORM_I2C_DRV i2c, unsigned char v);


#endif
