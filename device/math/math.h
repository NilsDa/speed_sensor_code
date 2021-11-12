
#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include "sdk_config.h"
#include "app_util_platform.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	uint8_t length;
	int16_t* data;  // array of size length
	int32_t sum;
	uint8_t current_index;
	bool filled_once;
} ma_filter_t;

typedef struct sRankNode
{
    int16_t value;                      //sample value
    struct sRankNode *nextAge;    //pointer to next oldest value
    struct sRankNode *prevAge;    //pointer to previous old value
    struct sRankNode *nextValue;  //pointer to next smallest value
    struct sRankNode *prevValue;  //pointer to previous smallest value
}sRankNode_t;

typedef struct
{
    uint16_t numNodes;          //rank node buffer length currently in use
	uint16_t numNodesMax;		//rank node maximal buffer length 
    sRankNode_t *rankBuffer;    //rank node buffer
    sRankNode_t *ageTail;         //pointer to oldest value
	sRankNode_t *unusedHead;		// pointer to head of unused nodes
    sRankNode_t *valueHead;       //pointer to smallest value
    sRankNode_t *valueMedian;      //pointer to median value
    sRankNode_t *valueTail;      //pointer to max value
}sRankFilter_t;

int16_t mean(int16_t *arr, uint16_t length);
int32_t variance(int16_t *arr, int16_t m, uint16_t length);
int16_t max(int16_t *arr, uint16_t length);
int16_t min(int16_t *arr, uint16_t length);

void ma_filter_init(ma_filter_t* fltr, uint8_t len);
void ma_filter_uninit(ma_filter_t* fltr);
int16_t ma_filter_step(ma_filter_t* fltr, int16_t val);
void ma_filter_reset(ma_filter_t* fltr);

bool RANKFILTER_Init(sRankFilter_t *rankFilter, int16_t nodeCount);
int16_t* RANKFILTER_Step(sRankFilter_t *rankFilter, int16_t sample);
void RANKFILTER_UpdateNumNodes(sRankFilter_t *rankFilter, int16_t count);

uint16_t atan2(int16_t x, int16_t y);
uint16_t atan20(int16_t x, int16_t y);

//static bool math_memory_initialized = false;

#ifdef __cplusplus
}
#endif

#endif //MATH_H

