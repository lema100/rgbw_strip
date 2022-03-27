#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "string.h"

#include "mb_slave_uart.h"
#include "update.h"
#include "arch.h"

#define MB_COUNT_MAX 1024
uint16_t ModBusSlaveData[MB_COUNT_MAX];
uint16_t default_par[MB_SETT_MAX - MB_SETT0] =
{
	0,			//	start not meter setting
	11520,		//	MB baudrate
	1,			//	MB slave
};

ModBusSlaveStruct ModBusSlave =
{
	.QuantReg = MB_COUNT_MAX,
	.SlaveAddrr = 1,
	.StopBit = 1,
	.Parity = 0,
	.MainClockUSART = 24000000,
	.Direct_GPIO = RS485_RX_TX_GPIO_Port,
	.Tx_GPIO = RS485_TX_GPIO_Port,
	.Rx_GPIO = RS485_RX_GPIO_Port,
	.Direct_pin = RS485_RX_TX_Pin_pos,
	.Tx_pin = RS485_TX_Pin_pos,
	.Rx_pin = RS485_RX_Pin_pos,
	.BaudRate = 115200,
	.Usart = USART3,
	.Tim = TIM2,
	.Data = ModBusSlaveData,
};

void SystemClock_Config(void);

void md_data_handling(void)
{
	TIM1->CCR1 = ModBusSlaveData[0];
	TIM1->CCR2 = ModBusSlaveData[1];
	TIM1->CCR3 = ModBusSlaveData[2];
	TIM1->CCR4 = ModBusSlaveData[3];

	uint8_t save = 0;
	for (uint32_t i = MB_SET_SETT0; i < MB_SET_SETT_MAX; i++)
	{
		if (ModBusSlaveData[i] != ModBusSlaveData[i - MB_SET_SETT0 + MB_SETT0])
		{
			ModBusSlaveData[i - MB_SET_SETT0 + MB_SETT0] = ModBusSlaveData[i];
			save = 1;
		}
	}
	if (save)
		arch_save_par((uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2);

	if (ModBusSlaveData[MB_RESET])
		NVIC_SystemReset();

	uint32_t address = ModBusSlaveData[MB_UPDATE_ADDR] * 4 + FLASH_BASE;
	if (address >= UPDATE_START && address < UPDATE_END)
	{
		switch(ModBusSlaveData[MB_UPDATE_CMD])
		{
		case UPDATE_CMD_READ:
		{
			memcpy(&ModBusSlaveData[MB_UPDATE_DATA], address, ModBusSlaveData[MB_UPDATE_LEN]);
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_WRITE:
		{
			HAL_FLASH_Unlock();
			for (uint32_t i = 0; i < ModBusSlaveData[MB_UPDATE_LEN]; i += 4)
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, *(uint32_t *)(((uint8_t *)&ModBusSlaveData[MB_UPDATE_DATA]) + i));
			HAL_FLASH_Lock();
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_ERASE:
		{
			FLASH_EraseInitTypeDef pEraseInit;
			uint32_t PageError;
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.NbPages = 1;
			pEraseInit.PageAddress = address;

			HAL_FLASH_Unlock();
			HAL_FLASHEx_Erase(&pEraseInit, &PageError);
			HAL_FLASH_Lock();
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_CHECK:
		{
			update_ctx_t su;
			ModBusSlaveData[MB_UPDATE_CMD] = update_check(&su);
			break;
		}
		default:
			break;
		}
	}
	else
		ModBusSlaveData[MB_UPDATE_CMD] |= 0x4000;
}

int main(void)
{
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_TIM1_Init();
	if (!arch_get_par((uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2) ||
		HAL_GPIO_ReadPin(RS485_DEFAULT_GPIO_Port, RS485_DEFAULT_Pin) == GPIO_PIN_SET)
		memcpy((uint8_t *)&ModBusSlaveData[MB_SETT0], (uint8_t *)default_par, (MB_SETT_MAX - MB_SETT0) * 2);

	memcpy((uint8_t *)&ModBusSlaveData[MB_SET_SETT0], (uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2);
	strcpy((char *)&ModBusSlaveData[MB_VERSION], VERSION " " __DATE__);

	ModBusSlave.BaudRate = ModBusSlaveData[MB_SETT_MB_BAUD] * 10;
	ModBusSlave.SlaveAddrr = ModBusSlaveData[MB_SETT_MB_SLAVE];

	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);

	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();

	HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	ModBusSlaveInit(&ModBusSlave);

	while (1)
	{
		ModBusSlaveHandling(&ModBusSlave);
		md_data_handling();

		if (HAL_GetTick() / 100 % 2)
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
															|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{

	__disable_irq();
	while (1)
	{
	}
}

#ifdef	USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}

#endif /* USE_FULL_ASSERT */

