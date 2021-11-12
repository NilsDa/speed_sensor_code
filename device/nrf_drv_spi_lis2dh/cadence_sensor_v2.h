#ifndef CADENCE_SENSOR_V2_H
#define CADENCE_SENSOR_V2_H

#include "sdk_config.h"
#include "app_util_platform.h"

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
} cadence_data_t;
	
	
void cadence_sensor_v2_init(void (*cb)(cadence_data_t data));
void cadence_sensor_v2_start(void);
void cadence_sensor_v2_stop(void);
void cadence_sensor_v2_get(uint16_t *cad);
void cadence_sensor_v2_uninit(void);
	
#ifdef __cplusplus
}
#endif

#endif  // CADENCE_SENSOR_V2_H

