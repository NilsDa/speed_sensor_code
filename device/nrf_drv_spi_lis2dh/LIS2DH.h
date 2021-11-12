

#ifndef LIS2DH_H
#define LIS2DH_H

#include "sdk_config.h"


#ifdef __cplusplus
extern "C" {
#endif


/** @snippet [LIS2DH read/write access] */
#ifndef		LIS2DH_SPI_SINGLE_READ
#define		LIS2DH_SPI_SINGLE_READ				0x80 // 0x01
#endif

#ifndef		LIS2DH_SPI_MULTI_READ_AUTOINC
#define		LIS2DH_SPI_MULTI_READ_AUTOINC		0xC0 // 0x03
#endif

#ifndef		LIS2DH_SPI_SINGLE_WRITE
#define		LIS2DH_SPI_SINGLE_WRITE				0x00 // 0x00
#endif

#ifndef		LIS2DH_SPI_MULTI_WRITE_AUTOINC
#define		LIS2DH_SPI_MULTI_WRITE_AUTOINC		0x40 // 0x02
#endif

/** @snippet [LIS2DH read/write access] */


/** @snippet [LIS2DH register addresses] */

//read
#ifndef		LIS2DH_STATUS_REG_AUX
#define		LIS2DH_STATUS_REG_AUX		0x07
#endif

//read
#ifndef		LIS2DH_WHO_AM_I
#define		LIS2DH_WHO_AM_I                 0x0F
#endif


//read/write
#ifndef		LIS2DH_CTRL_REG1
#define 	LIS2DH_CTRL_REG1			0x20
#endif

//read/write
#ifndef		LIS2DH_CTRL_REG2
#define 	LIS2DH_CTRL_REG2			0x21
#endif

//read/write
#ifndef		LIS2DH_CTRL_REG3
#define 	LIS2DH_CTRL_REG3			0x22
#endif

//read/write
#ifndef		LIS2DH_CTRL_REG4
#define 	LIS2DH_CTRL_REG4			0x23
#endif

//read/write
#ifndef		LIS2DH_CTRL_REG5
#define 	LIS2DH_CTRL_REG5			0x24
#endif

//read/write
#ifndef		LIS2DH_CTRL_REG6
#define 	LIS2DH_CTRL_REG6			0x25
#endif

//read/write
#ifndef		LIS2DH_REFERENCE
#define 	LIS2DH_REFERENCE			0x26
#endif

//read
#ifndef		LIS2DH_STATUS_REG2
#define 	LIS2DH_STATUS_REG2			0x27
#endif

//read
#ifndef		LIS2DH_OUT_X_L
#define 	LIS2DH_OUT_X_L				0x28
#endif

//read
#ifndef		LIS2DH_OUT_X_H
#define 	LIS2DH_OUT_X_H				0x29
#endif

//read
#ifndef		LIS2DH_OUT_Y_L
#define 	LIS2DH_OUT_Y_L				0x2A
#endif

//read
#ifndef		LIS2DH_OUT_Y_H
#define 	LIS2DH_OUT_Y_H				0x2B
#endif

//read
#ifndef		LIS2DH_OUT_Z_L
#define 	LIS2DH_OUT_Z_L				0x2C
#endif

//read
#ifndef		LIS2DH_OUT_Z_H
#define 	LIS2DH_OUT_Z_H				0x2D
#endif

//read/write
#ifndef		LIS2DH_FIFO_CTRL_REG
#define 	LIS2DH_FIFO_CTRL_REG		0x2E
#endif

//read
#ifndef		LIS2DH_FIFO_SRC_REG
#define 	LIS2DH_FIFO_SRC_REG			0x2F
#endif

//read/write
#ifndef		LIS2DH_INT1_CFG
#define 	LIS2DH_INT1_CFG				0x30
#endif

//read
#ifndef		LIS2DH_INT1_SRC
#define 	LIS2DH_INT1_SRC				0x31
#endif

//read/write
#ifndef		LIS2DH_INT1_THS
#define 	LIS2DH_INT1_THS				0x32
#endif

//read/write
#ifndef		LIS2DH_INT1_DURATION
#define 	LIS2DH_INT1_DURATION		0x33
#endif

//read/write
#ifndef		LIS2DH_INT2_CFG
#define 	LIS2DH_INT2_CFG				0x34
#endif

//read
#ifndef		LIS2DH_INT2_SRC
#define 	LIS2DH_INT2_SRC				0x35
#endif

//read/write
#ifndef		LIS2DH_INT2_THS
#define 	LIS2DH_INT2_THS				0x36
#endif

//read/write
#ifndef		LIS2DH_INT2_DURATION
#define 	LIS2DH_INT2_DURATION		0x37
#endif

//read/write
#ifndef		LIS2DH_CLICK_CFG
#define 	LIS2DH_CLICK_CFG			0x38
#endif

//read
#ifndef		LIS2DH_CLICK_SOURCE
#define 	LIS2DH_CLICK_SOURCE			0x39
#endif

//read/write
#ifndef		LIS2DH_CLICK_THS
#define 	LIS2DH_CLICK_THS			0x3A
#endif

//read/write
#ifndef		LIS2DH_TIME_LIMIT
#define 	LIS2DH_TIME_LIMIT			0x3B
#endif

//read/write
#ifndef		LIS2DH_TIME_LATENCY
#define 	LIS2DH_TIME_LATENCY			0x3C
#endif

//read/write
#ifndef		LIS2DH_TIME_WINDOW
#define 	TIME_WINDOW					0x3D
#endif

//read/write
#ifndef		LIS2DH_Act_THS
#define 	LIS2DH_Act_THS				0x3E
#endif

//read/write
#ifndef		LIS2DH_Act_DUR
#define 	LIS2DH_Act_DUR				0x3F
#endif
/** @snippet [LIS2DH register addresses] */



#ifdef __cplusplus
}
#endif


#endif //LIS2DH_H
