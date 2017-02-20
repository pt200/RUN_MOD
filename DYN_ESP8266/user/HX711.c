// Bondarenko Andrey

#include "osapi.h"
#include "user_interface.h"
#include "HX711.h"


#define GAIN		3
#define DOUT_PIN	2
#define SCK_PIN		0


void ICACHE_FLASH_ATTR _HX711_WAIT()
{
	os_delay_us( 1);
}
void ICACHE_FLASH_ATTR _HX711_SCK_0()
{
	_HX711_WAIT();
	GPIO_OUTPUT_SET( SCK_PIN, 0);
}
void ICACHE_FLASH_ATTR _HX711_SCK_1()
{
	_HX711_WAIT();
	GPIO_DIS_OUTPUT( SCK_PIN);
}
void ICACHE_FLASH_ATTR _HX711_DOUT_1()
{
	GPIO_DIS_OUTPUT( DOUT_PIN);
}
int ICACHE_FLASH_ATTR _HX711_GET_DOUT()
{
	return GPIO_INPUT_GET( DOUT_PIN);
}

int32_t ICACHE_FLASH_ATTR HX711_get_data() {
	int32_t ret = 0;
	int nbit;

	//Wait data
	//ToDo: timeout -> error
	while( _HX711_GET_DOUT() == 1){}

	// Receive data
	for( nbit = 0; nbit < 24; nbit++)
	{
		ret <<= 1;

		_HX711_SCK_1();
		_HX711_SCK_0();
		if(_HX711_GET_DOUT())
			ret |= 1;
	}

	//Send config
	for( nbit = 0; nbit < GAIN; nbit++)
	{
		_HX711_SCK_1();
		_HX711_SCK_0();
	}

	if( ret & 0x00800000)
		ret |= 0xFF000000;

	return ret;
}

void ICACHE_FLASH_ATTR HX711_Init() {
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	gpio_output_set(0, 0, ( 1 << SCK_PIN), ( 1 << DOUT_PIN));
}
