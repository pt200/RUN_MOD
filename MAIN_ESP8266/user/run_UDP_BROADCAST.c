// Bondarenko Andrey

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "math.h"
//#include <assert.h>
#include "driver/tm1637.h"



#define STANBY_TIMEOUT	120
#define RUN_SENSOR_PIN	12
#define LOCK_RST_PIN	14

void* tm1637_up = NULL;
void* tm1637_dn = NULL;
int32_t dist_mm = 0;


//extern int ets_uart_printf(const char *fmt, ...);
void ICACHE_FLASH_ATTR RUN_SendData(uint8_t *buff, int len);

//Listening connection data
LOCAL struct espconn RUN_Conn;

LOCAL os_timer_t pm_task_timer;
//LOCAL void* data_q = NULL;

void gpio_intr_handler( void *arg){
	static uint32 _st = 0;

	char buf[ 16];
	uint32 gpio_status;
	uint32 st;


	ETS_GPIO_INTR_DISABLE();
	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

st = system_get_time();
if( ( st - _st) > 20000)
{
dist_mm += 180;
tm1637_show_int( tm1637_up, ( dist_mm / 1000));
os_sprintf( buf, "%d", dist_mm);
RUN_SendData( buf, os_strlen( buf));
_st = st;
}

	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR RUN_SendData(uint8_t *buff, int len) {
	espconn_sent( &RUN_Conn, (uint8_t*) buff, len);
}

//Power management task
void ICACHE_FLASH_ATTR pm_task(void)
{
	static int timeout = 0;
	static int32_t _dist_mm = -1;
	int32_t dist_mm_tmp = dist_mm;

tm1637_show_int( tm1637_dn, ( ( 360 * ( dist_mm_tmp - _dist_mm) / 5000)));


	if( _dist_mm != dist_mm_tmp)
	{
		_dist_mm = dist_mm_tmp;
		timeout = 0;
	}
	else
	{
		if( ++timeout >= ( STANBY_TIMEOUT / 5))
		{
			ETS_GPIO_INTR_DISABLE();
			tm1637_bright( tm1637_up, 0); // Off LED display
			tm1637_bright( tm1637_dn, 0); // Off LED display
			//Unlock RST pin
			system_deep_sleep( 0); // Always. Need reset pulse.
		}
	}
}

//Initialize listening socket, do general initialization
void ICACHE_FLASH_ATTR RUN_Init(int port) {

//	wifi_set_broadcast_if(STATIONAP_MODE); // send UDP broadcast from both station and soft-AP interface

	RUN_Conn.type = ESPCONN_UDP;
	RUN_Conn.proto.udp = ( esp_udp*)os_zalloc( sizeof( esp_udp));
	RUN_Conn.proto.udp->local_port = 49088;  // set a available  port

	const char udp_remote_ip[4] = {255, 255, 255, 255};
	os_memcpy( RUN_Conn.proto.udp->remote_ip, udp_remote_ip, 4); // ESP8266 udp remote IP
	RUN_Conn.proto.udp->remote_port = 49088;  // ESP8266 udp remote port

//	espconn_regist_recvcb( &DYN_Conn, user_udp_recv_cb); // register a udp packet receiving callback
//	espconn_regist_sentcb( &DYN_Conn, user_udp_sent_cb); // register a udp packet sent callback

	espconn_create( &RUN_Conn);   // create udp

	// Start pump "task"
	os_timer_disarm( &pm_task_timer);
	os_timer_setfn( &pm_task_timer, ( os_timer_func_t*)pm_task, NULL);
	os_timer_arm( &pm_task_timer, 5000, 1);

	tm1637_up = tm1637_init( I2C_DRV( GPIO_PIN5, GPIO_PIN4), 40);
	tm1637_dn = tm1637_init( I2C_DRV( GPIO_PIN0, GPIO_PIN2), 40);


	tm1637_set_vram_symbol( tm1637_dn, 3, '-');
	tm1637_update( tm1637_dn);

	tm1637_set_vram_symbol( tm1637_up, 3, '-');
	tm1637_update( tm1637_up);

	// Interrupt move sensor
	ETS_GPIO_INTR_ATTACH( gpio_intr_handler, 0);
	PIN_FUNC_SELECT( PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	gpio_pin_intr_state_set( GPIO_ID_PIN( RUN_SENSOR_PIN), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();
}
