#include "cadence_sensor_v2.h"
#include "nrf_drv_lis2dh.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "math.h"

//#ifndef INT1_PIN
//#define INT1_PIN 25
//#endif
//#ifndef INT2_PIN
//#define INT2_PIN 26
//#endif

#define MIN_FILTER_LEN 9
#define DEFAULT_FILTER_LEN 25
#define MAX_FILTER_LEN 49
//#define D_DEGREE_FILTER_LEN 5
#define MIN_CADENCE 15
#define MAX_CADENCE 200
#define MIN_CADENCE_MAX_TURN_COUNTER (60000 / MIN_CADENCE / CADENCE_SENSOR_UPDATE_INTERVALL)
#define MAX_CADENCE_MIN_TURN_COUNTER (60000 / MAX_CADENCE / CADENCE_SENSOR_UPDATE_INTERVALL)

#define APP_TIMER_TICKS_TO_MS(TICKS)							\
            ((uint32_t)ROUNDED_DIV(                        		\
            (TICKS) * 1000 * (APP_TIMER_CONFIG_RTC_FREQUENCY + 1),	\
            (uint64_t)APP_TIMER_CLOCK_FREQ))

enum
{
	CADENCE_SENSOR_STATE_NONE = 0,
	CADENCE_SENSOR_STATE_INITIALIZED,
	CADENCE_SENSOR_STATE_RUNNING,
};

//static nrf_drv_lis2dh_int_pin_config_t pin_int1_cfg = NRF_DRV_LIS2DH_INT_PIN_DISABLE_CONFIG;
static nrf_drv_lis2dh_config_t sensor_cfg = NRF_DRV_LIS2DH_DEFAULT_CONFIG;

// allocate memory for app timer instance
APP_TIMER_DEF(m_app_timer_id);

// current orientation
uint16_t m_orientation_angle;

// current cadence in RPM
static int16_t m_cadence;
static int16_t m_cadence_filtered;

// counter for cadence calculation from full trun detection
static int16_t m_full_turn_cadence;
static int16_t m_int_degree;
static uint16_t m_full_turn_counter;

// filters used to calculate cadence
static ma_filter_t m_ma_filter_cad;
static ma_filter_t m_ma_filter_x, m_ma_filter_y;
static sRankFilter_t m_rank_filter_x, m_rank_filter_y/*, m_rank_filter_d_degree*/;
static sRankNode_t m_rank_buffer_x[MAX_FILTER_LEN], m_rank_buffer_y[MAX_FILTER_LEN]/*, m_rank_buffer_d_degree[D_DEGREE_FILTER_LEN]*/;

// sensor state
static uint8_t m_state;

// callback for sensor measurement
static void (*measurement_cb)(cadence_data_t data);


static cadence_data_t compute_cadence_data(int16_t* data)
{
	int16_t* tmp;
	int16_t mean_x = ma_filter_step(&m_ma_filter_x, data[0]);
	int16_t mean_y = ma_filter_step(&m_ma_filter_y, data[1]);
	tmp = RANKFILTER_Step(&m_rank_filter_x, data[0]);
	int16_t min_x = tmp[0];
	int16_t max_x = tmp[2];
	tmp = RANKFILTER_Step(&m_rank_filter_y, data[1]);
	int16_t min_y = tmp[0];
	int16_t max_y = tmp[2];
	
	// process raw data
	int16_t comp_x = (int16_t)(((int32_t)(data[0] - mean_x)) * 1024 / (max_x - min_x));
	int16_t comp_y = (int16_t)(((int32_t)(data[1] - mean_y)) * 1024 / (max_y - min_y));
	
	bool full_turn_pos_detected = false;
	uint16_t orientation = atan20(comp_x, comp_y);
	//uint16_t orientation = atan20(data[0], data[1]);
	int16_t d_degree = abs(orientation - m_orientation_angle);
	if(d_degree >= 1800)
	{ma_filter_step(&m_ma_filter_y, data[1]);
		d_degree = 3600 - d_degree;
		if(orientation > m_orientation_angle)
			d_degree = -d_degree;  // flip sign
		
		// in this case sensor is in 0/360° position
		full_turn_pos_detected = true;
	}
	else if(orientation < m_orientation_angle)
		d_degree = -d_degree;  // flip sign
	m_orientation_angle = orientation;
	// filter d_degree using median filter
	// tmp = RANKFILTER_Step(&m_rank_filter_d_degree, d_degree);
	// d_degree = tmp[1];  // use median
	
	// calculate cadence form full turn detections
	bool full_turn_detected = false;
	if(full_turn_pos_detected || abs(m_int_degree) >= 3600)
	{
		if(m_full_turn_counter >= MAX_CADENCE_MIN_TURN_COUNTER)
		{
			// full turn is completed with high confidence => calculate cadence
			full_turn_detected = true;
			m_full_turn_cadence = (60000 / m_full_turn_counter / CADENCE_SENSOR_UPDATE_INTERVALL);
			if(m_cadence_filtered < 0)
				m_full_turn_cadence = -m_full_turn_cadence;
		}
		else if(m_full_turn_counter > MIN_CADENCE_MAX_TURN_COUNTER)
		{
			// in this case it is too fast to complete a full turn
			// => noise and/or holding the pedal in 0/360° position could have caused this trigger
			m_full_turn_cadence = 0;
		}
		
		m_int_degree = 0;
		m_full_turn_counter = 0;
	}
	else
	{
		m_int_degree += d_degree;
		m_full_turn_counter++;
	}
	
	//m_cadence = (d_degree * 60000 / CADENCE_SENSOR_UPDATE_INTERVALL / 360 / 10);
	m_cadence = (d_degree * 50 / 3 / CADENCE_SENSOR_UPDATE_INTERVALL);
	m_cadence_filtered = ma_filter_step(&m_ma_filter_cad, m_cadence);
	
	return (cadence_data_t){{data[0], data[1], data[2]}, orientation, CADENCE_SENSOR_UPDATE_INTERVALL, d_degree, m_cadence, m_cadence_filtered, m_full_turn_cadence, full_turn_detected, comp_x, comp_y};
}

static void fifo_read_cb(int16_t* raw)
{
	cadence_data_t data = compute_cadence_data(raw);
	
	if(measurement_cb != NULL)
	{
		measurement_cb(data);
	}
}
static void app_timer_handler_continuous(void * p_context)
{
	nrf_drv_lis2dh_read_fifo(fifo_read_cb);
}


/**@brief Create timers.
 */
static void create_timers()
{
    ret_code_t err_code;
    err_code = app_timer_create(&m_app_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                app_timer_handler_continuous);
    APP_ERROR_CHECK(err_code);
}

void cadence_sensor_v2_init(void (*cb)(cadence_data_t data))
{
	m_state = CADENCE_SENSOR_STATE_NONE;
	
	// app timer utils setup
	app_timer_init();
	create_timers();
	
	// LIS2DH sensor config
	sensor_cfg.disable_axis = NRF_DRV_LIS2DH_DISABLE_Z_AXIS_FLAG;
	sensor_cfg.data_rate = NRF_DRV_LIS2DH_DR_25HZ;
	sensor_cfg.mode = NRF_DRV_LIS2DH_MODE_NORMAL;
	sensor_cfg.measurement_range = NRF_DRV_LIS2DH_FS_4G;
	sensor_cfg.enable_stw_rts = false;
	
	// LIS2DH initialization
	nrf_drv_lis2dh_init(&sensor_cfg);
    // NRF_LOG_INFO("LIS2DH init done.");
	
	// register callback
	measurement_cb = cb;
	
	// Moving average filter initializations
	ma_filter_init(&m_ma_filter_cad, CADENCE_SENSOR_MA_FILTER_LENGTH);
	ma_filter_init(&m_ma_filter_x, DEFAULT_FILTER_LEN);
	ma_filter_init(&m_ma_filter_y, DEFAULT_FILTER_LEN);
	
	// Rank filter initializations
	bool success = true;
	m_rank_filter_x.numNodesMax = MAX_FILTER_LEN;
	m_rank_filter_x.rankBuffer = m_rank_buffer_x;
	success = success && RANKFILTER_Init(&m_rank_filter_x, DEFAULT_FILTER_LEN);
	m_rank_filter_y.numNodesMax = MAX_FILTER_LEN;
	m_rank_filter_y.rankBuffer = m_rank_buffer_y;
	success = success && RANKFILTER_Init(&m_rank_filter_y, DEFAULT_FILTER_LEN);
	// m_rank_filter_d_degree.numNodesMax = D_DEGREE_FILTER_LEN;
	// m_rank_filter_d_degree.rankBuffer = m_rank_buffer_d_degree;
	// success = success && RANKFILTER_Init(&m_rank_filter_d_degree, D_DEGREE_FILTER_LEN);
	
	if(success)
		m_state = CADENCE_SENSOR_STATE_INITIALIZED;
}

void cadence_sensor_v2_start(void)
{
	if(m_state < CADENCE_SENSOR_STATE_INITIALIZED)
		return;
	
	m_cadence = 0;
	m_cadence_filtered = 0;
	
	// initialize full turn counter
	m_full_turn_cadence = 0;
	m_int_degree = 0;
	m_full_turn_counter = 0;
	
	// Reset moving average filter
	ma_filter_reset(&m_ma_filter_cad);
	
	// Start app timer to continuously measure sensor orientation
	ret_code_t err_code = app_timer_start(m_app_timer_id, APP_TIMER_TICKS(CADENCE_SENSOR_UPDATE_INTERVALL), NULL);
	APP_ERROR_CHECK(err_code);
	
	m_state = CADENCE_SENSOR_STATE_RUNNING;
}

void cadence_sensor_v2_stop(void)
{
	ret_code_t err_code = app_timer_stop(m_app_timer_id);
    APP_ERROR_CHECK(err_code);
	m_cadence = 0;
	
	// TODO: put LIS2DH12 into power down mode => implement power down function in driver
	
	m_state = CADENCE_SENSOR_STATE_INITIALIZED;
}

void cadence_sensor_v2_get(uint16_t *cad)
{
//  *cad = m_cadence_filtered;
  *cad = m_cadence;
}

void cadence_sensor_v2_uninit(void)
{
	if(m_state > CADENCE_SENSOR_STATE_INITIALIZED)
		cadence_sensor_v2_stop();
		
	ma_filter_uninit(&m_ma_filter_cad);
	
	m_state = CADENCE_SENSOR_STATE_NONE;
}














