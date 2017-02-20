// Bondarenko Andrey

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "math.h"
//#include <assert.h>

#include "HX711.h"
#include "queue.h"


extern int ets_uart_printf(const char *fmt, ...);

void ICACHE_FLASH_ATTR DYN_SendData(uint8_t *buff, int len);
int32_t ICACHE_FLASH_ATTR HX711_get_avg();
int ICACHE_FLASH_ATTR DYN_calib_weight( float clb_w);
int ICACHE_FLASH_ATTR DYN_calib_load();
int ICACHE_FLASH_ATTR DYN_calib_save();


//Listening connection data
LOCAL struct espconn DYN_Conn;

LOCAL os_timer_t pump_task_timer;
LOCAL void* data_q = NULL;


#define DYN_CMD_TARE			'0'
#define DYN_CMD_CAL_ZERO		'c'
#define DYN_CMD_CAL_WEIGHT_X	'C'
#define DYN_CMD_CAL_DONE		'W'




#define AVG_RAW_BUF_SIZE		10
volatile int32_t _HX711_RAW_BUF[ AVG_RAW_BUF_SIZE];
volatile int _HX711_RAW_BUF_pos  = 0;

// Calibrate data
int32_t HX711_RAW_zero = 0;
float DYN_RAW_K = 1.0;
// User data
//float DYN_tare = 0;


static void ICACHE_FLASH_ATTR DYN_ConnRecvCb(void *espconn, char *data,
		unsigned short len) {
	if( len < 1)
		return;
	switch( *data)
	{
		case DYN_CMD_TARE:
//			DYN_tare = HX711_get_avg();
			break;
		case DYN_CMD_CAL_ZERO:
			HX711_RAW_zero = HX711_get_avg();
			break;
		case DYN_CMD_CAL_WEIGHT_X:
			DYN_calib_weight( 51.0);
			break;
		case DYN_CMD_CAL_DONE:
			DYN_calib_save();
			break;
	}
}

int32_t ICACHE_FLASH_ATTR HX711_get_avg()
{
	int q;
	int32_t ret = 0;
	for( q = 0; q < AVG_RAW_BUF_SIZE; q++)
		ret += _HX711_RAW_BUF[ q];
	return ( ret / AVG_RAW_BUF_SIZE);
}

void gpio_intr_handler( void *arg){
	int32_t HX711_RAW_data;
	uint32 gpio_status;

	ETS_GPIO_INTR_DISABLE();
	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);


	HX711_RAW_data = HX711_get_data();

	q32push_overwrite( data_q, HX711_RAW_data);


if( _HX711_RAW_BUF_pos >= AVG_RAW_BUF_SIZE)
	_HX711_RAW_BUF_pos = 0;
_HX711_RAW_BUF[ _HX711_RAW_BUF_pos++] = HX711_RAW_data;


	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR DYN_SendData(uint8_t *buff, int len) {
	espconn_sent(&DYN_Conn, (uint8_t*) buff, len);
}

int ICACHE_FLASH_ATTR DYN_calib_weight( float clb_w)
{
	float RAW_w = ( HX711_get_avg() - HX711_RAW_zero) * 1.0;
	if( clb_w > 0)
	{
		DYN_RAW_K = ( float)RAW_w / clb_w;
		return 0;
	}
	return 1;
}

int ICACHE_FLASH_ATTR DYN_calib_load()
{
	HX711_RAW_zero = 0;
	DYN_RAW_K = 1.0;
	return 0;
}

int ICACHE_FLASH_ATTR DYN_calib_save()
{
	return 0;
}

LOCAL char _tx_buf[ 128 * 16 + 1];
void ICACHE_FLASH_ATTR pump_task(void)
{
	char buf2[ 16];
	int err;
	int32_t data;
int32_t n;

	_tx_buf[ 0] = 0;

	for( n = 0; n < 128; n++)
	{
		ETS_GPIO_INTR_DISABLE();
		err = q32pop( data_q, &data);
		ETS_GPIO_INTR_ENABLE();

		if( err != 0)
			break;

		os_sprintf( buf2, "%d\n", data);
		os_strcat( _tx_buf, buf2);
	}

	if( _tx_buf[ 0] != 0)// !!! "strlen() > 0" !!!
		DYN_SendData( (uint8_t*) _tx_buf, strlen( _tx_buf));
}

//Initialize listening socket, do general initialization
void ICACHE_FLASH_ATTR DYN_Init(int port) {

//	wifi_set_broadcast_if(STATIONAP_MODE); // send UDP broadcast from both station and soft-AP interface

	DYN_Conn.type = ESPCONN_UDP;
	DYN_Conn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	DYN_Conn.proto.udp->local_port = 49087;  // set a available  port

	const char udp_remote_ip[4] = {255, 255, 255, 255};
	os_memcpy(DYN_Conn.proto.udp->remote_ip, udp_remote_ip, 4); // ESP8266 udp remote IP
	DYN_Conn.proto.udp->remote_port = 49087;  // ESP8266 udp remote port

//	espconn_regist_recvcb( &DYN_Conn, user_udp_recv_cb); // register a udp packet receiving callback
//	espconn_regist_sentcb( &DYN_Conn, user_udp_sent_cb); // register a udp packet sent callback

	espconn_create( &DYN_Conn);   // create udp

	DYN_calib_load();

	HX711_Init();

	//Create data queue
	data_q = q32create( 128);

	// Start pump "task"
	os_timer_disarm( &pump_task_timer);
	os_timer_setfn( &pump_task_timer, ( os_timer_func_t*)pump_task, NULL);
	os_timer_arm( &pump_task_timer, 200, 1);

	// Interrupt from HX711 @data ready@
	ETS_GPIO_INTR_ATTACH( gpio_intr_handler, 0);
	gpio_pin_intr_state_set( GPIO_ID_PIN( 2), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();
}
