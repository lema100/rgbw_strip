#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define VERSION "V.1.1"

void Error_Handler(void);

#define RS485_TX_Pin_pos 		10
#define RS485_RX_Pin_pos 		11
#define RS485_RX_TX_Pin_pos		5

#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOC
#define ADC_VCC_Pin GPIO_PIN_5
#define ADC_VCC_GPIO_Port GPIOA
#define ADC_G_Pin GPIO_PIN_6
#define ADC_G_GPIO_Port GPIOA
#define ADC_R_Pin GPIO_PIN_7
#define ADC_R_GPIO_Port GPIOA
#define ADC_B_Pin GPIO_PIN_0
#define ADC_B_GPIO_Port GPIOB
#define ADC_W_Pin GPIO_PIN_1
#define ADC_W_GPIO_Port GPIOB
#define RS485_TX_Pin GPIO_PIN_10
#define RS485_TX_GPIO_Port GPIOB
#define RS485_RX_Pin GPIO_PIN_11
#define RS485_RX_GPIO_Port GPIOB
#define PWM_W_Pin GPIO_PIN_8
#define PWM_W_GPIO_Port GPIOA
#define PWM_B_Pin GPIO_PIN_9
#define PWM_B_GPIO_Port GPIOA
#define PWM_R_Pin GPIO_PIN_10
#define PWM_R_GPIO_Port GPIOA
#define PWM_G_Pin GPIO_PIN_11
#define PWM_G_GPIO_Port GPIOA
#define RS485_RX_TX_Pin GPIO_PIN_5
#define RS485_RX_TX_GPIO_Port GPIOB
#define RS485_DEFAULT_Pin GPIO_PIN_6
#define RS485_DEFAULT_GPIO_Port GPIOB

enum
{
	MB_PWM_W = 0,
	MB_PWM_B,
	MB_PWM_G,
	MB_PWM_R,
	MB_PWM2_W,
	MB_PWM2_B,
	MB_PWM2_G,
	MB_PWM2_R,
	MB_SETT0 = 50,
	MB_SETT_MB_BAUD,
	MB_SETT_MB_SLAVE,
	MB_SETT3,
	MB_SETT4,
	MB_SETT5,
	MB_SETT6,
	MB_SETT7,
	MB_SETT8,
	MB_SETT_MAX,
	MB_VERSION = 80,
	MB_RESET = 99,
	MB_SET_SETT0 = 100,
	MB_SET_SETT_MB_BAUD,
	MB_SET_SETT_MB_SLAVE,
	MB_SET_SETT3,
	MB_SET_SETT4,
	MB_SET_SETT5,
	MB_SET_SETT6,
	MB_SET_SETT7,
	MB_SET_SETT8,
	MB_SET_SETT_MAX,
	MB_UPDATE_CMD = 256,
	MB_UPDATE_ADDR,
	MB_UPDATE_LEN,
	MB_UPDATE_DATA,
};

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
