/*
 *  Example of working sensor DHT22 (temperature and humidity) and send data on the service thingspeak.com
 *  https://thingspeak.com
 *
 *  For a single device, connect as follows:
 *  DHT22 1 (Vcc) to Vcc (3.3 Volts)
 *  DHT22 2 (DATA_OUT) to ESP Pin GPIO2
 *  DHT22 3 (NC)
 *  DHT22 4 (GND) to GND
 *
 *  Between Vcc and DATA_OUT need to connect a pull-up resistor of 10 kOh.
 *
 *  (c) 2015 by Mikhail Grigorev <sleuthhound@gmail.com>
 *
 */

#include <user_interface.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <os_type.h>
#include "driver/uart.h"
#include "driver/gpio16.h"
#include "user_config.h"
#include "RUN.h"


extern int ets_uart_printf(const char *fmt, ...);
int (*console_printf)(const char *fmt, ...) = ets_uart_printf;


/*

LOCAL void ICACHE_FLASH_ATTR setup_wifi_st_mode(void);

static struct ip_info ipConfig;
static ETSTimer WiFiLinker;
//static tConnState connState = WIFI_CONNECTING;
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	os_timer_disarm(&WiFiLinker);
	switch(wifi_station_get_connect_status())
	{
		case STATION_GOT_IP:
			wifi_get_ip_info(STATION_IF, &ipConfig);
			if(ipConfig.ip.addr != 0) {
//				connState = WIFI_CONNECTED;
				_DEBUG("WiFi connected, wait DHT22 timer...\r\n");
			} else {
//				connState = WIFI_CONNECTING_ERROR;
				_DEBUG("WiFi connected, ip.addr is null\r\n");
			}
			break;
		case STATION_WRONG_PASSWORD:
//			connState = WIFI_CONNECTING_ERROR;
			_DEBUG("WiFi connecting error, wrong password\r\n");
			break;
		case STATION_NO_AP_FOUND:
//			connState = WIFI_CONNECTING_ERROR;
			_DEBUG("WiFi connecting error, ap not found\r\n");
			break;
		case STATION_CONNECT_FAIL:
//			connState = WIFI_CONNECTING_ERROR;
			_DEBUG("WiFi connecting fail\r\n");
			break;
		default:
//			connState = WIFI_CONNECTING;
			_DEBUG("WiFi connecting...\r\n");
	}
	os_timer_setfn(&WiFiLinker, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&WiFiLinker, WIFI_CHECK_DELAY, 0);
}


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void)  // About 120ms after RST
{
//Lock RST pin
	gpio16_output_conf();
	gpio16_output_set( 0);
}




/******************************************************************************
 * FunctionName : user_set_softap_config
 * Description  : set SSID and password of ESP8266 softAP
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_set_softap_config(void)
{
   struct softap_config config;
 
   wifi_softap_get_config( &config); // Get config first.
    
   if( os_strcmp( config.ssid, AP_SSID) != 0)
   {
     os_memset(config.ssid, 0, 32);
     os_memset(config.password, 0, 64);
     os_memcpy(config.ssid, AP_SSID, os_strlen( AP_SSID));
     os_memcpy(config.password, AP_PASS, os_strlen( AP_PASS));
     config.authmode = AUTH_WPA_WPA2_PSK;
     config.ssid_len = 0;// or its actual length
     config.max_connection = 4; // how many stations can connect to ESP8266 softAP at most.
 
     wifi_softap_set_config(&config);// Set ESP8266 softap config .
     wifi_set_opmode( SOFTAP_MODE);
   }
}
 

void ICACHE_FLASH_ATTR user_init(void) // About 140ms after RST
{
/*
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	// Enable system messages
	system_set_os_print(1);
	ets_uart_printf("\n==== System info: ====\n");
	ets_uart_printf("SDK version:%s rom %d\n", system_get_sdk_version(), system_upgrade_userbin_check());
	ets_uart_printf("Time = %ld\n", system_get_time());
	ets_uart_printf("Chip id = 0x%x\n", system_get_chip_id());
	ets_uart_printf("CPU freq = %d MHz\n", system_get_cpu_freq());
	ets_uart_printf("Flash size map = %d\n", system_get_flash_size_map());
	ets_uart_printf("Free heap size = %d\n", system_get_free_heap_size());
	ets_uart_printf("==== End System info ====\n");
	ets_uart_printf("System init...\r\n");
*/

	// ESP8266 softAP set config.
	user_set_softap_config();


	RUN_Init( 0);

//	ets_uart_printf("System init done.\n");
}
