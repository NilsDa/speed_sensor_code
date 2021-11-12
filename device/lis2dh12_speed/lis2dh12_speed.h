#ifndef LIS2DH12_SPEED_H
#define LIS2DH12_SPEED_H


#include "sdk_config.h"
#include "app_util_platform.h"
#include "nrf_drv_lis2dh.h"

#ifdef __cplusplus
extern "C" {
#endif




typedef struct 
{
	int16_t raw[3];
	uint16_t orientation;
	uint16_t delta_ms;
	int16_t delta_degree;
	int16_t cadence;
	int16_t cadence_filtered;
	int16_t cadence_full_turn;
	bool full_turn_detected;
	int16_t x_comp;
	int16_t y_comp;
} speed_data_t;





void lis2dh12_speed_init(void (*cb)(speed_data_t data));
void lis2dh12_speed_measure();
// void lis2dh12_speed_get_raw(int16_t (*accel)[3]);
void lis2dh12_speed_start(void);
void lis2dh12_speed_stop(void);
void lis2dh12_speed_uninit(void);






#ifdef __cplusplus
}
#endif


#endif // !LIS2DH12_SPEED_H