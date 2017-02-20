// Bondarenko Andrey

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "HX711.h"
#include "math.h"

//#include <assert.h>

extern int ets_uart_printf(const char *fmt, ...);

//Listening connection data
static struct espconn DYN_Conn;
static esp_tcp DYN_Tcp;

void ICACHE_FLASH_ATTR DYN_SendData(uint8_t *buff, int len);
int32_t ICACHE_FLASH_ATTR HX711_get_avg();
int ICACHE_FLASH_ATTR DYN_calib_weight( float clb_w);
int ICACHE_FLASH_ATTR DYN_calib_load();
int ICACHE_FLASH_ATTR DYN_calib_save();


enum DYN_OUTPUT_TYPE
{
  DO_HUMAN,
  DO_CSV
};

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
float DYN_tare = 0;

uint8_t DYN_cfg = DO_CSV;


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

static void ICACHE_FLASH_ATTR DYN_ConnSentDoneCb(void *espconn) {
}

static void ICACHE_FLASH_ATTR DYN_ConnCb(void *espconn) {
//	char* msg = "DYN_v1\n\r\x1B[0;35m MAGENTA\r\n \x1B[0;31m RED";
//	espconn_sent(espconn, (uint8_t*) msg, 39);
}

static void ICACHE_FLASH_ATTR DYN_ConnReconCb(void *espconn, sint8 err) {
}

static void ICACHE_FLASH_ATTR DYN_ConnDisconCb(void *espconn) {
}

void ICACHE_FLASH_ATTR DYN_SendData(uint8_t *buff, int len) {
	espconn_sent(&DYN_Conn, (uint8_t*) buff, len);
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
	char buf[ 32];

	ETS_GPIO_INTR_DISABLE();
	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	HX711_RAW_data = HX711_get_data();

	if( _HX711_RAW_BUF_pos >= AVG_RAW_BUF_SIZE)
		_HX711_RAW_BUF_pos = 0;
	_HX711_RAW_BUF[ _HX711_RAW_BUF_pos++] = HX711_RAW_data;

//	os_sprintf( buf, "\x1B[0;0f\x1B[1;32mDYN_v1\n\rRAW ADC = %d      \r\nRAW = %d      ",
os_sprintf( buf, "%d\r\n", (int32_t)( HX711_RAW_data - HX711_RAW_zero));
DYN_SendData( (uint8_t*) buf, strlen( buf));


	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	ETS_GPIO_INTR_ENABLE();
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

//Initialize listening socket, do general initialization
void ICACHE_FLASH_ATTR DYN_Init(int port) {

	DYN_Conn.type = ESPCONN_TCP;
	DYN_Conn.state = ESPCONN_NONE;
	DYN_Conn.proto.tcp = &DYN_Tcp;

	DYN_Tcp.local_port = port;

	espconn_tcp_set_max_con_allow(&DYN_Conn, 1);
	espconn_regist_connectcb(&DYN_Conn, DYN_ConnCb);
	espconn_regist_disconcb(&DYN_Conn, DYN_ConnDisconCb);
    espconn_regist_reconcb(&DYN_Conn, DYN_ConnReconCb);
	espconn_regist_recvcb(&DYN_Conn, DYN_ConnRecvCb);
	espconn_regist_sentcb(&DYN_Conn, DYN_ConnSentDoneCb);

	espconn_accept(&DYN_Conn);

	DYN_calib_load();

	HX711_Init();

	ETS_GPIO_INTR_ATTACH( gpio_intr_handler, 0);
	gpio_pin_intr_state_set( GPIO_ID_PIN( 2), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();
}
