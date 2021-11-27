
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_rtt.h"


#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"



#define FILE_NAME   "Data.TXT"
#define TEST_STRING "X Y Z\n"



/**
 * @brief  SDC block device definition
 * */
 /*
NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);
*/



//static void fatfs_example();