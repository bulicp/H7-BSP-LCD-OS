/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PA3_SDRAM_H
#define __PA3_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"




/* Private define ------------------------------------------------------------*/
#define SDRAM_ROWS						 4096
#define SDRAM_COLS						 256
#define SDRAM_BUFFER_SIZE        ((uint32_t)0x0100)   		// 256
#define MAT_ROWS			     ((uint32_t)64)
#define N 1
#define MDMA_BLOCK_SIZE			 (MAT_ROWS*SDRAM_COLS)/2    // 32 KB block transfer

/**
  * @brief  SDRAM status structure definition
  */
#define   SDRAM_OK         ((uint8_t)0x00)
#define   SDRAM_ERROR      ((uint8_t)0x01)

/** @defgroup STM32F769I_DISCOVERY_SDRAM_Exported_Constants SDRAM Exported Constants
  * @{
  */


/* LCD has its Frame Buffer located at the start of SDRAM (0xD0000000)
 * LCD width=480, LCD height = 272
 * Frame buffer is of size 0x0007F7FC (522236 Bytes)
 * Hence, use the address 0xD0080000 for SDRAM read/write to avoid writing in the Frame Buffer
 * Pa3cio, 11.12.2022
 */
#define PA3_SDRAM_DEVICE_ADDR_RW  	((uint32_t)0xD0000000) + 0x00080000
#define PA3_SDRAM_DEVICE_SIZE  		((uint32_t)0x01000000)  /* SDRAM device size in MBytes */


/*
 * SDRAM Refresha rate: 64 ms, 4096 rows: 15.625 us/row
 * SDRAM clk = 10 ns, refresh count = 15.625us/10ns = 1562
 * Refresh count = 1562 - 22 (safe margin) = 1540
 */
#define SDRAM_REFRESH_COUNT              ((uint32_t)1540)   /* SDRAM refresh counter (100Mhz SDRAM clock) */

#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/** @defgroup SDRAM Exported Functions
  * @{
  */
uint8_t Init_SDRAM(void);
void SDRAM_Initialization_sequence(uint32_t RefreshCount);
void simple_SDRAM_test(void);


void SDRAM_init_matrices(void);
void SDRAM_write_matrix(void);
uint8_t SDRAM_compare_matrices(void);
void SDRAM_mat_row_access_test(void);
void SDRAM_mat_col_access_test(void);
void SDRAM_DMA_mat_row_access_test(void);
void SDRAM_MDMA_mat_row_access_test(void);



#endif

