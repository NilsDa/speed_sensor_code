#ifndef NRF_DRV_LIS2DH_H
#define NRF_DRV_LIS2DH_H

#include "sdk_config.h"
//#include "watty_v03.h"
#include "app_util_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

	
/************************************************************************************************/
/*	Definitions and type declarations															*/
/************************************************************************************************/
#define LIS2DH_SPI_CONFIG									 \
{                                                            \
    .sck_pin      = SPIM0_SCK_PIN,                			 \
    .mosi_pin     = SPIM0_MOSI_PIN,                			 \
    .miso_pin     = SPIM0_MISO_PIN,                			 \
    .ss_pin       = SPIM0_SS_PIN,                				 \
    .irq_priority = 7,         								 \
    .orc          = 0x00,                                    \
    .frequency    = NRF_DRV_SPI_FREQ_8M,                     \
    .mode         = NRF_DRV_SPI_MODE_0,                      \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,         \
}

typedef enum
{
	NRF_DRV_LIS2DH_MODE_LP = 0,	// low power mode
	NRF_DRV_LIS2DH_MODE_NORMAL, // normal mode
	NRF_DRV_LIS2DH_MODE_HR		// high resolution mode
	
} nrf_drv_lis2dh_mode_t;

typedef enum
{
	NRF_DRV_LIS2DH_FS_2G = 0,	// +-2g
	NRF_DRV_LIS2DH_FS_4G, 		// +-4g
	NRF_DRV_LIS2DH_FS_8G, 		// +-8g
	NRF_DRV_LIS2DH_FS_16G 		// +-16g
	
} nrf_drv_lis2dh_measurement_range_t;

typedef enum
{
	NRF_DRV_LIS2DH_FIFO_MODE_BYPASS = 0,	// bypass fifo (no fifo)
	NRF_DRV_LIS2DH_FIFO_MODE_FIFO,			// fifo mode (stop collecting data when fifo full)
	NRF_DRV_LIS2DH_FIFO_MODE_STREAM,		// stream mode (when fifo full discard older data)
	NRF_DRV_LIS2DH_FIFO_MODE_STREAM_TO_FIFO	// stream-to-fifo mode (like stream mode until event triggers, then switch to fifo mode)
} nrf_drv_lis2dh_fifo_mode_t;

typedef enum
{
	NRF_DRV_LIS2DH_DR_POWER_DOWN = 0,		// Power down mode
	NRF_DRV_LIS2DH_DR_1HZ,					// HR / normal / Low power mode (1 Hz)
	NRF_DRV_LIS2DH_DR_10HZ, 				// HR / normal / Low power mode (10 Hz)
	NRF_DRV_LIS2DH_DR_25HZ, 				// HR / normal / Low power mode (25 Hz)
	NRF_DRV_LIS2DH_DR_50HZ, 				// HR / normal / Low power mode (50 Hz)
	NRF_DRV_LIS2DH_DR_100HZ, 				// HR / normal / Low power mode (100 Hz)
	NRF_DRV_LIS2DH_DR_200HZ, 				// HR / normal / Low power mode (200 Hz)
	NRF_DRV_LIS2DH_DR_400HZ, 				// HR/ normal / Low power mode (400 Hz)
	NRF_DRV_LIS2DH_DR_1620HZ_LP, 			// Low power mode (1.620 kHz)
	NRF_DRV_LIS2DH_DR_5376HZ_LP_1344HZ_HR 	// HR/ normal (1.344 kHz);
											// Low power mode (5.376 kHz)
	
} nrf_drv_lis2dh_data_rate_t;

#define NRF_DRV_LIS2DH_DISABLE_X_AXIS_FLAG (1 << 0)	// disable x axis measurement
#define NRF_DRV_LIS2DH_DISABLE_Y_AXIS_FLAG (1 << 1)	// disable y axis measurement
#define NRF_DRV_LIS2DH_DISABLE_Z_AXIS_FLAG (1 << 2)	// disable z axis measurement

#define NRF_DRV_LIS2DH_INT_D4D_FLAG (1 << 0)		// enables D4D interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_CLICK_FLAG (1 << 1)		// (not implemented) enables CLICK interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_BOOT_FLAG (1 << 2)		// (not implemented) enables CLICK interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_ACTIVITY_FLAG (1 << 3)	// enables ACTIVITY interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_XL_FLAG (1 << 4)			// enables XL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_XH_FLAG (1 << 5)			// enables XH interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_YL_FLAG (1 << 6)			// enables YL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_YH_FLAG (1 << 7)			// enables YH interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_ZL_FLAG (1 << 8)			// enables ZL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_INT_ZH_FLAG (1 << 9)			// enables ZH interrupt on INT1/2 pin


#define NRF_DRV_LIS2DH_EVENT_XL_FLAG (1 << 0)	// event for XL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_XH_FLAG (1 << 1)	// event for XH interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_YL_FLAG (1 << 2)	// event for YL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_YH_FLAG (1 << 3)	// event for YH interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_ZL_FLAG (1 << 4)	// event for ZL interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_ZH_FLAG (1 << 5)	// event for ZH interrupt on INT1/2 pin
#define NRF_DRV_LIS2DH_EVENT_IA_FLAG (1 << 6)	// event for IA interrupt on INT1/2 pin

typedef struct
{
	bool enable;		// enable INT on corresponding pin
	uint8_t pin;		// GPIO pin connected to INTn signal
	uint16_t flags;		// assign interrupts for corresponding pin
	uint8_t threshold;	// INTn_THS threshold value
	uint8_t duration;	// INTn_DUR duration value
	void (*event_handler)(uint8_t event);	// event handler
} nrf_drv_lis2dh_int_pin_config_t;

typedef struct 
{
	nrf_drv_lis2dh_mode_t mode;  // specify measurement mode
	uint8_t disable_axis;  // use defined flags to disable x-, y- & z-axis individually
	nrf_drv_lis2dh_measurement_range_t measurement_range;  // specifiy measurement range
	nrf_drv_lis2dh_fifo_mode_t fifo_mode;  // specify fifo mode
	nrf_drv_lis2dh_data_rate_t data_rate;  // specify data rate and power down mode
	nrf_drv_lis2dh_int_pin_config_t int1_cfg;  // specify int1 pin configuration
	nrf_drv_lis2dh_int_pin_config_t int2_cfg;  // specify int2 pin configuration
	bool enable_stw_rts;  // enable �Sleep to wake� and �Return to sleep� functionaliy
} nrf_drv_lis2dh_config_t;

/************************************************************************************************/
/*	Default definitions																					*/
/************************************************************************************************/

#define NRF_DRV_LIS2DH_INT_DEFAULT_DURATION 5	// (INTn_DURATION) D6 - D0 bits set the minimum duration of the Interrupt 1
//													event to be recognized. Duration
//													steps and maximum values depend on the ODR chosen.
//													Duration time is measured in N/ODR, where N is the content
//													of the duration register.

#define NRF_DRV_LIS2DH_INT_DEFAULT_THRESHOLD 60	// specify INTn_THS<6:0>
//							1LSb = 16mg @FS=2g
//							1LSb = 32 mg @FS=4g
//							1LSb = 62 mg @FS=8g
//							1LSb = 186 mg @FS=16g

#define NRF_DRV_LIS2DH_DEFAULT_DURATION_VALUE 5
#define NRF_DRV_LIS2DH_DEFAULT_THRESHOLD_VALUE 10
#define NRF_DRV_LIS2DH_MAX_DURATION_VALUE 0x7F
#define NRF_DRV_LIS2DH_MAX_THRESHOLD_VALUE 0x7F


#define NRF_DRV_LIS2DH_INT_PIN_DISABLE_CONFIG		\
{													\
	.enable 		= false,						\
	.pin			= 0,							\
	.flags			= 0,							\
	.threshold		= 0,							\
	.duration		= 0,							\
	.event_handler	= NULL							\
}

#define NRF_DRV_LIS2DH_DEFAULT_CONFIG								\
{																	\
	.mode 				= NRF_DRV_LIS2DH_MODE_LP,					\
	.disable_axis		= 0,										\
	.measurement_range	= NRF_DRV_LIS2DH_FS_2G,						\
	.fifo_mode			= NRF_DRV_LIS2DH_FIFO_MODE_BYPASS,			\
	.data_rate			= NRF_DRV_LIS2DH_DR_100HZ,					\
	.int1_cfg			= NRF_DRV_LIS2DH_INT_PIN_DISABLE_CONFIG,	\
	.int2_cfg			= NRF_DRV_LIS2DH_INT_PIN_DISABLE_CONFIG,	\
	.enable_stw_rts 	= true										\
}



/************************************************************************************************/
/*	Functions																					*/
/************************************************************************************************/
void nrf_drv_lis2dh_init(nrf_drv_lis2dh_config_t *cfg);
bool nrf_drv_lis2dh_read_fifo(void (*done_cb)(int16_t* data));





#ifdef __cplusplus
}
#endif

// #endif  // NRF_DRV_LIS2DH_ENABLE

#endif  // NRF_DRV_LIS2DH_H

