#ifndef __SDRAM_H__
#define __SDRAM_H__

#include "stm32f4xx_hal.h"

#define Bank5_SDRAM_ADDR	((uint32_t)(0xc0000000))		// SDRAM开始地址

// SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1					((uint16_t) 0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2					((uint16_t) 0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4					((uint16_t) 0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8					((uint16_t) 0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL				((uint16_t) 0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED			((uint16_t) 0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2						((uint16_t) 0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3						((uint16_t) 0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD			((uint16_t) 0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED		((uint16_t) 0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE			((uint16_t) 0x0200)


extern SDRAM_HandleTypeDef SDRAM_Handler;


uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint8_t regval);
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);
void SDRAM_Init(void);
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram);
#endif
