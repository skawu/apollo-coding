#include "sdram.h"

SDRAM_HandleTypeDef SDRAM_Handler;		// SDRAM���

// ��SDRAM��������
// bankx:0,��Bank5�����SDRAM����ָ��
//		 1,��Bank6�����SDRAM����ָ��
// cmd:ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
// refresh:�Զ�ˢ�´���
// regval:ģʽ�Ĵ����Ķ���
// ����ֵ: 0,���� 1,ʧ��
uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint8_t regval)
{
	uint32_t target_bank = 0;
	FMC_SDRAM_CommandTypeDef Command;
	
	if(bankx == 0)	
		target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
	else if(bankx == 1)
		target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.CommandMode = cmd;						// ����
	Command.CommandTarget = target_bank;			// Ŀ��SDRAM�洢����
	Command.AutoRefreshNumber = refresh;			// �Զ�ˢ�´���
	Command.ModeRegisterDefinition = regval;		// Ҫд��ģʽ�Ĵ�����ֵ
	
	if(HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, 0x1000) == HAL_OK)	// ��SDRAM��������
		return 0;
	else 
		return 1;
}

// ��ָ����ַ(WriteAddr + Bank5_SDRAM_ADDR)��ʼ������д��n���ֽ�
// pBuffer:�ֽ�ָ��
// WriteAddr:Ҫд��ĵ�ַ
// n:Ҫд����ֽ���
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n)
{
	for(; n != 0; n--)
	{
		*(__IO uint8_t *)(Bank5_SDRAM_ADDR + WriteAddr) = *pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

// ����SDRAM��ʼ������
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
	uint32_t temp = 0;
	uint32_t delay = 18000 * 5;
	
	// SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);			// ʱ������ʹ��
	while(delay--);												// ��ʱ����200us���˴���ʱ500us
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);				// �����д洢��Ԥ���
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);	// �����Զ�ˢ�´���
	
	// ����ģʽ�Ĵ�����SDRAM��bit0~bit2Ϊָ��ͻ�����ʳ���
		// bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7~bit8Ϊ����ģʽ
		// bit9Ϊָ����дͻ��ģʽ��bit10��bit11Ϊ����λ
	
	temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1			|		// ����ͻ�����ȣ�1��������1/2/4/8��
					SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL		|		// ����ͻ�����ͣ�����������������/����
					SDRAM_MODEREG_CAS_LATENCY_3				|		// ����CASֵ��3��������2/3��
					SDRAM_MODEREG_OPERATING_MODE_STANDARD 	|		// ���ò���ģʽ��0����׼ģʽ
					SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;			// ����SDRAM��ģʽ�Ĵ���
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);
	
	// ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���)�����㷽����
		// COUNT = SDRAMˢ������ / ���� - 20 = SDRAMˢ������(us) * SDCLKƵ��(MHz) / ���� - 20
	// ʹ�õ�SDRAM��������Ϊ64ms��SDCLK = 180 / 2 = 90MHz,����Ϊ8192(2^13)
		// COUNT = 64 * 1000 * 90 / 8192 -20 = 683
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, 683);
}

// SDRAM��ʼ��
void SDRAM_Init(void)
{
	FMC_SDRAM_TimingTypeDef SDRAM_Timing;
	
	SDRAM_Handler.Instance = FMC_SDRAM_DEVICE;								// SDRAM��BANK5,6
	SDRAM_Handler.Init.SDBank = FMC_SDRAM_BANK1;							// SDRAM��BANK5
	SDRAM_Handler.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;		// ������
	SDRAM_Handler.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;			// ������
	SDRAM_Handler.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;		// ���ݿ��Ϊ16bit
	SDRAM_Handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;	// �ڲ�����4��Bank
	SDRAM_Handler.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;				// CAS �е�ַѡͨ�źţ�3��
	SDRAM_Handler.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;// ʧ��д����
	SDRAM_Handler.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;			// SDRAMʱ��ΪHCLK/2=180M/2=90M=11.1ns
	SDRAM_Handler.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;					// ʹ��ͻ��
	SDRAM_Handler.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;				// ��ͨ����ʱ
	
	SDRAM_Timing.LoadToActiveDelay = 2;										// ����ģʽ�Ĵ���������ʱ�����ʱΪ����ʱ������
	SDRAM_Timing.ExitSelfRefreshDelay = 8;									// �˳��Զ�ˢ���ӳ�Ϊ8��ʱ������
	SDRAM_Timing.SelfRefreshTime = 6;										// ��ˢ��ʱ��Ϊ6��ʱ������
	SDRAM_Timing.RowCycleDelay = 6;											// ��ѭ���ӳ�Ϊ6��ʱ������
	SDRAM_Timing.WriteRecoveryTime = 2;										// �ָ���ʱΪ2��ʱ������
	SDRAM_Timing.RPDelay = 2;												// ��Ԥ�����ʱΪ2��ʱ������
	SDRAM_Timing.RCDDelay = 2;												// �е����ӳ�Ϊ2��ʱ������
	
	HAL_SDRAM_Init(&SDRAM_Handler, &SDRAM_Timing);
	
	// ����SDRAM��ʼ������
	SDRAM_Initialization_Sequence(&SDRAM_Handler); 
}


// SDRAM�ײ��������������ã�ʱ��ʹ��
// �˺�������HAL_SDRAM_Init()����
// hsdram:SDRAM���
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_FMC_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF12_FMC;			// ����ΪFMC
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);			// ��ʼ��GPIOC0,2,3
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
						GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);			// ��ʼ��GPIOD0,1,8,9,10,14,15
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
						GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | 
						GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);			// ��ʼ��GPIOE0,1,7,8,9,10,11,12,13,14,15
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
						GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | 
						GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);			// ��ʼ��GPIOE0,1,2,3,4,5,11,12,13,14,15
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 |
						GPIO_PIN_8 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_Initure);			// ��ʼ��GPIOE0,1,2,4,5,8,15
}
