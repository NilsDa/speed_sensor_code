#include "lis2dh12_speed.h"
//#include "nrf_drv_lis2dh.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_rtt.h"

#include "math.h"

//#include "SEGGER_RTT.h"



// set sensor update interval in ms. should fit configured sensor data rate (100Hz -> 10ms)
#ifndef SPEED_SENSOR_UPDATE_INTERVALL
#define SPEED_SENSOR_UPDATE_INTERVALL 10
#endif

enum	// enum for use with m_state
{
	SPEED_SENSOR_STATE_NONE = 0,
	SPEED_SENSOR_STATE_INITIALIZED,
	SPEED_SENSOR_STATE_RUNNING,
};

static uint8_t m_state;


static nrf_drv_lis2dh_config_t sensor_cfg = NRF_DRV_LIS2DH_DEFAULT_CONFIG;

// allocate memory for app timer instance
APP_TIMER_DEF(m_app_timer_id);

static void (*measurement_cb)(speed_data_t data);


//static volatile uint8_t counter_app_timer_handler = 0;
//static volatile uint8_t counter_fifo_read_cb = 0;



/*
static speed_data_t calc_speed_data(int16_t* data)
{





  return {{data[0], data[1], data[2]}, orientation, delta_ms, delta_degree, cadence, cadence_filtered, cadence_full_turn, full_turn_detected, x_comp, y_comp}
}
*/



static void fifo_read_cb(int16_t* raw)
{
	//counter_fifo_read_cb++;
	//SEGGER_RTT_printf(0,"%d -> fifo_read_cb -> %d;%d;%d\n",counter_fifo_read_cb,raw[0],raw[1],raw[2]);
	//SEGGER_RTT_printf(0,"fifo_read_cb aufgerufen!\n");

	// if (counter_fifo_read_cb >= 10)
	// {
	// 	SEGGER_RTT_printf(0,"counter_fifo... >=10\n");
	// }

	speed_data_t data = {{raw[0], raw[1], raw[2]}, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	


	if(measurement_cb != NULL)
	{
		measurement_cb(data);
	}

}



void lis2dh12_speed_measure()
{
	nrf_drv_lis2dh_read_fifo(fifo_read_cb);
}


static void app_timer_handler_continuous(void * p_context)
{
	//counter_app_timer_handler++;

	//SEGGER_RTT_printf(0,"%d -> app_timer_handler_continuous aufgerufen!\n",counter_app_timer_handler);

	nrf_drv_lis2dh_read_fifo(fifo_read_cb);
}

static void create_timers()
{
	ret_code_t err_code;
	err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, app_timer_handler_continuous);
	APP_ERROR_CHECK(err_code);

	//SEGGER_RTT_printf(0, "timer created.\n");
        NRF_LOG_INFO("timer created.\n");
}



void lis2dh12_speed_init(void (*cb)(speed_data_t data))
{
    // init speed_sensor state to none
    m_state = SPEED_SENSOR_STATE_NONE;




	// app timer utils setup
	app_timer_init();
	create_timers();




    // LIS2DH sensor config
	//sensor_cfg.disable_axis = NRF_DRV_LIS2DH_DISABLE_X_AXIS_FLAG;
	sensor_cfg.disable_axis = 0;
	sensor_cfg.data_rate = NRF_DRV_LIS2DH_DR_100HZ;
	sensor_cfg.mode = NRF_DRV_LIS2DH_MODE_NORMAL;
	sensor_cfg.measurement_range = NRF_DRV_LIS2DH_FS_2G;
	//sensor_cfg.measurement_range = NRF_DRV_LIS2DH_FS_16G;
	sensor_cfg.enable_stw_rts = false;
	
	// LIS2DH initialization
	nrf_drv_lis2dh_init(&sensor_cfg);


	// register callback
	measurement_cb = cb;


	// TODO: Filter initializations (moving average and rank filters)



	m_state = SPEED_SENSOR_STATE_INITIALIZED;
	
	
	NRF_LOG_INFO("init complete.\n");
}



void lis2dh12_speed_start(void)
{
	if(m_state < SPEED_SENSOR_STATE_INITIALIZED)
		return;
	
	// TODO: initialize all counters and calculated values to 0

	// TODO: reset moving average filter



	// Start app timer to continuously measure
	ret_code_t err_code = app_timer_start(m_app_timer_id, APP_TIMER_TICKS(SPEED_SENSOR_UPDATE_INTERVALL), NULL);
	APP_ERROR_CHECK(err_code);

	m_state = SPEED_SENSOR_STATE_RUNNING;


	NRF_LOG_INFO("started sensor.\n");
}

void lis2dh12_speed_stop(void)
{
	ret_code_t err_code = app_timer_stop(m_app_timer_id);
	APP_ERROR_CHECK(err_code);

	// Kommentar von Manu: TODO: put LIS2DH12 into power down mode => implement power down function in driver

	m_state = SPEED_SENSOR_STATE_INITIALIZED;
}

void lis2dh12_speed_uninit(void)
{
	if(m_state > SPEED_SENSOR_STATE_INITIALIZED)
		lis2dh12_speed_stop();
	
	// TODO: uninit moving average filter

	m_state = SPEED_SENSOR_STATE_NONE;
}