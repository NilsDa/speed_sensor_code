
#include "sdk_config.h"
#if NRF_DRV_LIS2DH_ENABLED

#include "LIS2DH.h"
#include "nrf_drv_lis2dh.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_gpiote.h"
// #include "app_util_platform.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SPI_INSTANCE  0  // SPI instance index

#define MAX_RX_BYTES 16	// maximum allowed RX bytes in the buffer
#define MAX_TX_BYTES 2	// maximum allowed TX bytes in the buffer

#if NRF_DRV_LIS2DH_DEBUG_ENABLED
static const uint8_t NRF_DRV_LIS2DH_EVENT_COUNT = 6;
#if NRF_DRV_LIS2DH_DEBUG_ENABLED
static const char* NRF_DRV_LIS2DH_INT_SRC_DEBUG_STRING[NRF_DRV_LIS2DH_EVENT_COUNT] =
	{ "XL", "XH", "YL", "YH", "ZL", "ZH" };
#endif
static const uint8_t NRF_DRV_LIS2DH_EVENTS[NRF_DRV_LIS2DH_EVENT_COUNT] =
{ 
	NRF_DRV_LIS2DH_EVENT_XL_FLAG,
	NRF_DRV_LIS2DH_EVENT_XH_FLAG,
	NRF_DRV_LIS2DH_EVENT_YL_FLAG,
	NRF_DRV_LIS2DH_EVENT_YH_FLAG,
	NRF_DRV_LIS2DH_EVENT_ZL_FLAG,
	NRF_DRV_LIS2DH_EVENT_ZH_FLAG
};
#endif

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  // SPI driver instance
//static nrf_drv_spi_config_t spi_config = LIS2DH_SPI_CONFIG;  // SPI configuration

//static uint8_t data_mode_shift;
static int16_t data_mode_divisor;

static uint8_t tx_buf[MAX_TX_BYTES]; 
static uint8_t rx_buf[MAX_TX_BYTES + MAX_RX_BYTES];
static uint8_t tx_len;  // holds the actual number of bytes to transmit
static uint8_t rx_len;  // holds the actual number of bytes to receive

static volatile bool spi_busy = false;  // flag to indicate if transfer is in progress


static uint8_t int1_pin, int2_pin;  // GPIO pins for INT1/2 connections


static void (*int1_event_handler)(uint8_t event);  // event handler attached to interrupts at INT1 pin
static void (*int2_event_handler)(uint8_t event);  // event handler attached to interrupts at INT2 pin

typedef enum
{
	LIS2DH_DRV_STATE_IDLE,
	LIS2DH_DRV_STATE_INT1_READING,
	LIS2DH_DRV_STATE_INT2_READING,
	LIS2DH_DRV_STATE_FIFO_READING
} lis2dh_drv_state_t;
volatile lis2dh_drv_state_t m_state;

// fifo read callback
static int16_t fifo_data[3];  // stored as {X, Y, Z}
void (*fifo_read_done_cb)(int16_t* data);

// prototypes
static void nrf_drv_lis2dh_write(const uint8_t reg_adr, const uint8_t val, bool block);
static void nrf_drv_lis2dh_read(const uint8_t reg_adr, const uint8_t len, bool block);
static void nrf_drv_lis2dh_transfer(const uint8_t *tx_buffer, const uint8_t tx_length,
	const uint8_t rx_length, bool block);



void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_busy = false;
	
	switch(m_state)
	{
		case LIS2DH_DRV_STATE_INT1_READING:
			// call event handler
			if(int1_event_handler != NULL)
			{
				uint8_t events = rx_buf[tx_len] & (~NRF_DRV_LIS2DH_EVENT_IA_FLAG);
#if NRF_DRV_LIS2DH_DEBUG_ENABLED
//				NRF_LOG_HEXDUMP_INFO(rx_buf + tx_len, rx_len);
				for(uint8_t i = 0; i < NRF_DRV_LIS2DH_EVENT_COUNT; i++)
				{
					if(events & NRF_DRV_LIS2DH_EVENTS[i])
						NRF_LOG_INFO(NRF_DRV_LIS2DH_INT_SRC_DEBUG_STRING[i]);
				}
#endif				
				int1_event_handler(events);
			}
			m_state = LIS2DH_DRV_STATE_IDLE;
			break;
		case LIS2DH_DRV_STATE_INT2_READING:
			// call event handler
			if(int2_event_handler != NULL)
			{
				uint8_t events = rx_buf[tx_len] & (~NRF_DRV_LIS2DH_EVENT_IA_FLAG);
#if NRF_DRV_LIS2DH_DEBUG_ENABLED
//				NRF_LOG_HEXDUMP_INFO(rx_buf + tx_len, rx_len);
				for(uint8_t i = 0; i < NRF_DRV_LIS2DH_EVENT_COUNT; i++)
				{
					if(events & NRF_DRV_LIS2DH_EVENTS[i])
						NRF_LOG_INFO(NRF_DRV_LIS2DH_INT_SRC_DEBUG_STRING[i]);
				}
#endif			
				int2_event_handler(events);
			}
			m_state = LIS2DH_DRV_STATE_IDLE;
			break;
			
		case LIS2DH_DRV_STATE_FIFO_READING:
//			fifo_data[0] = ((int16_t)(rx_buf[tx_len + 1] << 8) | (int16_t)(rx_buf[tx_len + 0])) >> data_mode_shift;
//			fifo_data[1] = ((int16_t)(rx_buf[tx_len + 3] << 8) | (int16_t)(rx_buf[tx_len + 2])) >> data_mode_shift;
//			fifo_data[2] = ((int16_t)(rx_buf[tx_len + 5] << 8) | (int16_t)(rx_buf[tx_len + 4])) >> data_mode_shift;
			fifo_data[0] = ((int16_t)(rx_buf[tx_len + 1] << 8) | (int16_t)(rx_buf[tx_len + 0])) / data_mode_divisor;
			fifo_data[1] = ((int16_t)(rx_buf[tx_len + 3] << 8) | (int16_t)(rx_buf[tx_len + 2])) / data_mode_divisor;
			fifo_data[2] = ((int16_t)(rx_buf[tx_len + 5] << 8) | (int16_t)(rx_buf[tx_len + 4])) / data_mode_divisor;
			if(fifo_read_done_cb != NULL)
				fifo_read_done_cb(fifo_data);
			m_state = LIS2DH_DRV_STATE_IDLE;
			break;
		
		case LIS2DH_DRV_STATE_IDLE:
			break;

	}
#if NRF_DRV_LIS2DH_DEBUG_ENABLED
	NRF_LOG_FLUSH();
#endif
}


void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
//#if NRF_DRV_LIS2DH_DEBUG_ENABLED
//	NRF_LOG_INFO("gpiote_event_handler()");
//#endif
	
	if(pin == int1_pin)
	{
		// NRF_LOG_INFO("Interrupt on INT1!");
		
		// read INT1_SRC register to clear IA bit
		m_state = LIS2DH_DRV_STATE_INT1_READING;
		nrf_drv_lis2dh_read(LIS2DH_INT1_SRC, 1, false);
	}
	else if(pin == int2_pin)
	{
		// NRF_LOG_INFO("Interrupt on INT2!");
		
		// read INT2_SRC register to clear IA bit
		m_state = LIS2DH_DRV_STATE_INT2_READING;
		nrf_drv_lis2dh_read(LIS2DH_INT2_SRC, 1, false);
	}
//#if NRF_DRV_LIS2DH_DEBUG_ENABLED
//	NRF_LOG_FLUSH();
//#endif
}


void nrf_drv_lis2dh_init(nrf_drv_lis2dh_config_t *cfg)
{
	ret_code_t err_code;
	
	// data divisor need to be initialized according to normal mode, since this is default
//	data_mode_shift = 6;  // 10 Bit data output -> divide by 2^(16-10)=2^6=64 => shift by 6
	data_mode_divisor = 64;
	
	// initialize SPI driver instance
	nrf_drv_spi_config_t spi_config = LIS2DH_SPI_CONFIG;
	err_code = nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);
	APP_ERROR_CHECK(err_code);
//	NRF_LOG_INFO("LIS2DH initialization of SPI done");
//	NRF_LOG_FLUSH();
	
	// set LIS2DH registers according to configuration
	uint8_t value;  // sensor uses 8 bit registers
	
	// CTRL_REG1: LP, data rate, disable axis
	value = 0x07;  // default
	value ^= cfg->disable_axis;  // disable x-, y- & z-axis
	if(cfg->mode == NRF_DRV_LIS2DH_MODE_LP)
	{
//		data_mode_shift = 8;  // 8 Bit data output -> divide by 2^(16-8)=2^8=256 => shift by 8
		data_mode_divisor = 256;
		value |= (1 << 3);  // set LP mode bit
	}
	value |= (cfg->data_rate << 4);  // set data rate (definition of enum corresponds to correct register values)
	nrf_drv_lis2dh_write(LIS2DH_CTRL_REG1, value, true);
//	NRF_LOG_INFO("LIS2DH initialization of CTRL_REG1 done");
//	NRF_LOG_FLUSH();
	
	// CTRL_REG3: AOI1 interrupt
	value = 0x00;  // default
	if((cfg->int1_cfg.flags & NRF_DRV_LIS2DH_INT_D4D_FLAG) && cfg->int1_cfg.enable)
		value |= (1 << 6);  // set I1_AOI1 bit
	nrf_drv_lis2dh_write(LIS2DH_CTRL_REG3, value, true);
//	NRF_LOG_INFO("LIS2DH initialization of CTRL_REG3 done");
//	NRF_LOG_FLUSH();
	
	// CTRL_REG4: Full Scale selection, High Resolution bit
	value = 0x00;  // default
	if(cfg->mode == NRF_DRV_LIS2DH_MODE_HR)
	{
//		data_mode_shift = 4;  // 12 Bit data output -> divide by 2^(16-12)=2^4=16 => shift by 4
		data_mode_divisor = 16;
		value |= (1 << 3);  // High Resolution bit
	}
	value |= (cfg->measurement_range << 4);  // Full Scale selection
	nrf_drv_lis2dh_write(LIS2DH_CTRL_REG4, value, true);
//	NRF_LOG_INFO("LIS2DH initialization of CTRL_REG3 done");
//	NRF_LOG_FLUSH();
	
	// CTRL_REG5: D4D_INTn  
	value = 0x00;  // default
	if((cfg->int1_cfg.flags & NRF_DRV_LIS2DH_INT_D4D_FLAG) && cfg->int1_cfg.enable)
		value |= (1 << 2);  // D4D_INT1 bit
	if((cfg->int2_cfg.flags & NRF_DRV_LIS2DH_INT_D4D_FLAG) && cfg->int2_cfg.enable)
		value |= (1 << 0);  // D4D_INT2 bit
	nrf_drv_lis2dh_write(LIS2DH_CTRL_REG5, value, true);
//	NRF_LOG_INFO("LIS2DH initialization of CTRL_REG5 done");
//	NRF_LOG_FLUSH();
	
	// FIFO_CTRL_REG: FIFO mode selection
	value = 0x00;  // default
	value |= (cfg->fifo_mode << 6);  // select FIFO mode
	nrf_drv_lis2dh_write(LIS2DH_FIFO_CTRL_REG, value, true);
//	NRF_LOG_INFO("LIS2DH initialization of FIFO_CTRL_REG done");
//	NRF_LOG_FLUSH();
	
	if(cfg->int1_cfg.enable)
	{
		// store pin number
		int1_pin = cfg->int1_cfg.pin;
		
		// register event handler
		int1_event_handler = cfg->int1_cfg.event_handler;
		
		//init INT1 pin as interrupt triggered on LO->HI change
		if(!nrf_drv_gpiote_is_init())
		{
			err_code = nrf_drv_gpiote_init();
			APP_ERROR_CHECK(err_code);
		}
		nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
		in_config.pull = NRF_GPIO_PIN_PULLDOWN;
		err_code = nrf_drv_gpiote_in_init(cfg->int1_cfg.pin, &in_config, gpiote_event_handler);
		APP_ERROR_CHECK(err_code);
		nrf_drv_gpiote_in_event_enable(cfg->int1_cfg.pin, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT1 pin done");
//		NRF_LOG_FLUSH();
		
		// INT1_CFG: AOI, 6D, ZHIE/ZUPE, ZLIE/ZDOWNE, YHIE/YUPE, YLIE/YDOWNE, XHIE/XUPE & XLIE/XDOWNE
		value = 0x00;  // default
		value |= (uint8_t)((cfg->int1_cfg.flags & 0xFF0) >> 4);  // shift by 4, since NRF_DRV_LIS2DH_INT_XL_FLAG referes to bit 0 bis is defined as (1 << 4)
		if(cfg->int1_cfg.flags & NRF_DRV_LIS2DH_INT_D4D_FLAG)
			// value |= (3 << 6);  // set AOI and 6D bits (6 direction position recognition)
			value |= (1 << 6);  // clear AOI and set 6D bits (6 direction movement recognition)
		nrf_drv_lis2dh_write(LIS2DH_INT1_CFG, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT1_CFG done");
//		NRF_LOG_FLUSH();
		
		// INT1_THS: Interrupt 1 threshold
		value = 0x00; // default
		value = cfg->int1_cfg.threshold > NRF_DRV_LIS2DH_MAX_THRESHOLD_VALUE ? 
			NRF_DRV_LIS2DH_MAX_THRESHOLD_VALUE : cfg->int1_cfg.threshold;  // set Interrupt 1 threshold value
		nrf_drv_lis2dh_write(LIS2DH_INT1_THS, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT1_THS done");
//		NRF_LOG_FLUSH();
		
		// INT1_DURATION: Interrupt 1 duration
		value = 0x00; // default
		value = cfg->int1_cfg.duration > NRF_DRV_LIS2DH_MAX_DURATION_VALUE ? 
			NRF_DRV_LIS2DH_MAX_DURATION_VALUE : cfg->int1_cfg.duration;  // set Interrupt 1 duration value
		nrf_drv_lis2dh_write(LIS2DH_INT1_DURATION, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT1_DURATION done");
//		NRF_LOG_FLUSH();
		
		// read INT1_SRC register to clear IA bit
		nrf_drv_lis2dh_read(LIS2DH_INT1_SRC, 1, true);
	}
	
	if(cfg->int2_cfg.enable)
	{
		// store pin number
		int2_pin = cfg->int2_cfg.pin;
		
		// register event handler
		int2_event_handler = cfg->int2_cfg.event_handler;
		
		//init INT2 pin as interrupt triggered on LO->HI change
		if(!nrf_drv_gpiote_is_init())
		{
			err_code = nrf_drv_gpiote_init();
			APP_ERROR_CHECK(err_code);
		}
		nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
		in_config.pull = NRF_GPIO_PIN_PULLDOWN;
		err_code = nrf_drv_gpiote_in_init(cfg->int2_cfg.pin, &in_config, gpiote_event_handler);
		APP_ERROR_CHECK(err_code);
		nrf_drv_gpiote_in_event_enable(cfg->int2_cfg.pin, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT2 pin done");
//		NRF_LOG_FLUSH();
		
		// CTRL_REG6: I2_INT2 Interrrupt 2 function, P2_ACT Activity Interrupt
		value = 0x00;  // default
		if(cfg->int2_cfg.flags & 0xFF)
			value |= (1 << 5);  // I2_INT2 bit => enable Interrrupt 2 function on INT2 pin
		if(cfg->int2_cfg.flags & NRF_DRV_LIS2DH_INT_ACTIVITY_FLAG)
			value |= (1 << 1);  // P2_ACT bit => enable Activity interrupt on INT2 pin.
		nrf_drv_lis2dh_write(LIS2DH_CTRL_REG6, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of CTRL_REG6 done");
//		NRF_LOG_FLUSH();
	
		// INT2_CFG: AOI, 6D, ZHIE/ZUPE, ZLIE/ZDOWNE, YHIE/YUPE, YLIE/YDOWNE, XHIE/XUPE & XLIE/XDOWNE
		value = 0x00;  // default
		value |= ((cfg->int2_cfg.flags & 0xFF0) >> 4);  // shift by 4, since NRF_DRV_LIS2DH_INT_XL_FLAG referes to bit 0 bis is defined as (1 << 4)
		if(cfg->int2_cfg.flags & NRF_DRV_LIS2DH_INT_D4D_FLAG)
			// value |= (3 << 6);  // set AOI and 6D bits (6 direction position recognition)
			value |= (1 << 6);  // clear AOI and set 6D bits (6 direction movement recognition)
		nrf_drv_lis2dh_write(LIS2DH_INT2_CFG, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT2_CFG done");
//		NRF_LOG_FLUSH();
		
		// INT2_THS: Interrupt 2 threshold
		value = 0x00; // default
		value = cfg->int2_cfg.threshold;  // set Interrupt 2 threshold value
		nrf_drv_lis2dh_write(LIS2DH_INT2_THS, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT2_THS done");
//		NRF_LOG_FLUSH();
		
		// INT2_DURATION: Interrupt 2 duration
		value = 0x00; // default
		value = cfg->int2_cfg.duration;  // set Interrupt 2 duration value
		nrf_drv_lis2dh_write(LIS2DH_INT2_DURATION, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of INT2_DURATION done");
//		NRF_LOG_FLUSH();
		
		// read INT2_SRC register to clear IA bit
		nrf_drv_lis2dh_read(LIS2DH_INT2_SRC, 1, true);
	}
	
	if(cfg->enable_stw_rts)
	{
		// Act_THS: Sleep to wake, return to Sleep activation threshold in Low power mode
		value = 0x00; // default
		value = NRF_DRV_LIS2DH_DEFAULT_THRESHOLD_VALUE;  // set activation threshold value
		nrf_drv_lis2dh_write(LIS2DH_Act_THS, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of Act_THS done");
//		NRF_LOG_FLUSH();
		
		// Act_DUR: Sleep to Wake, Return to Sleep duration
		value = 0x00; // default
		value = NRF_DRV_LIS2DH_DEFAULT_DURATION_VALUE;  // set activation duration value
		nrf_drv_lis2dh_write(LIS2DH_Act_DUR, value, true);
//		NRF_LOG_INFO("LIS2DH initialization of Act_DUR done");
//		NRF_LOG_FLUSH();
	}
	else
	{
		// set Act_THS and Act_DUR to default value (0x00) to disable Sleep-to-wake-return-to-sleep function
		value = 0x00; // default
		nrf_drv_lis2dh_write(LIS2DH_Act_THS, value, true);
		nrf_drv_lis2dh_write(LIS2DH_Act_DUR, value, true);
		
	}
	
	fifo_read_done_cb = NULL;
	m_state = LIS2DH_DRV_STATE_IDLE;
}

bool nrf_drv_lis2dh_read_fifo(void (*done_cb)(int16_t* data))
{
	if(m_state != LIS2DH_DRV_STATE_IDLE)
		return false;
	if(done_cb == NULL || done_cb != fifo_read_done_cb)
	{
		if(done_cb == NULL)
			return false;
		fifo_read_done_cb = done_cb;
	}
	
#if NRF_DRV_LIS2DH_DEBUG_ENABLED
    NRF_LOG_INFO("Reading FIFO...");
	NRF_LOG_FLUSH();
#endif
	
	m_state = LIS2DH_DRV_STATE_FIFO_READING;
	nrf_drv_lis2dh_read(LIS2DH_OUT_X_L, 6, false);
	
//	uint8_t buf[1] = {LIS2DH_OUT_X_L};
//	nrf_drv_lis2dh_transfer(buf, 1, 4 + 1, false);
	
	return true;
}


static void nrf_drv_lis2dh_write(const uint8_t reg_adr, const uint8_t val, bool block)
{
	const uint8_t buf[2] = { (LIS2DH_SPI_SINGLE_WRITE | reg_adr), val };
	nrf_drv_lis2dh_transfer(buf, 2, 2, block);
}
static void nrf_drv_lis2dh_read(const uint8_t reg_adr, const uint8_t len, bool block)
{
	uint8_t buf[1];
	if(len == 1)
		buf[0] = (LIS2DH_SPI_SINGLE_READ | reg_adr);
	else
		buf[0] = (LIS2DH_SPI_MULTI_READ_AUTOINC | reg_adr);
	nrf_drv_lis2dh_transfer(buf, 1, len + 1, block);
}
static void nrf_drv_lis2dh_transfer(const uint8_t *tx_buffer, const uint8_t tx_length,
	const uint8_t rx_length, bool block)
{
	while (spi_busy) __WFE();
		
	memcpy(tx_buf, tx_buffer, tx_length);
	tx_len = tx_length;
	rx_len = rx_length - tx_length;
	
	memset(rx_buf, 0, rx_len);
    spi_busy = true;
	
    APP_ERROR_CHECK(
		nrf_drv_spi_transfer(&spi, tx_buf, tx_length, rx_buf, rx_length)
	);
	
	if(block)
	{
		while (spi_busy) __WFE();
	}
}

















#endif  // NRF_DRV_LIS2DH_ENABLE
