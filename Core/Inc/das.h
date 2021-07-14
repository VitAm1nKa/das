#ifndef DAS_H_
#define DAS_H_
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include "net_utilities.h"
//--------------------------------------------------
// extern UART_HandleTypeDef huart2;
//--------------------------------------------------
#define SLIP_END 		0xC0
#define SLIP_ESC 		0xDB
#define SLIP_ESC_END 	0xDC
#define SLIP_ESC_ESC 	0xDD
//--------------------------------------------------
#define DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS 	be16toword(0x0000)
#define DAS_APP_FUNC_ERROR_ADC 				be16toword(0x0001)
#define DAS_APP_FUNC_ERROR_INV_LN_ID 		be16toword(0x0002)
#define DAS_APP_FUNC_ERROR_ARG				be16toword(0x0003)
#define DAS_APP_FUNC_ERROR_PLS_LN			be16toword(0x0006)

#define DAS_APP_FUNC_ECHO 					be16toword(0x0004)
#define DAS_APP_FUNC_ECHO_RESP 				be16toword(0x0005)
#define DAS_APP_FUNC_READ_ANALOG			be16toword(0x0008)
#define DAS_APP_FUNC_READ_ANALOG_RESP		be16toword(0x0009)
#define DAS_APP_FUNC_ANALOG_CFG 			be16toword(0x001A)
#define DAS_APP_FUNC_ANALOG_CFG_RESP 		be16toword(0x001B)
#define DAS_APP_FUNC_PULSE_CFG 				be16toword(0x0036)
#define DAS_APP_FUNC_PULSE_CFG_RESP 		be16toword(0x0037)
#define DAS_APP_FUNC_PULSE_READ 			be16toword(0x0038)
#define DAS_APP_FUNC_PULSE_READ_RESP 		be16toword(0x0039)
#define DAS_APP_FUNC_PULSE_CLEAR 			be16toword(0x0040)
#define DAS_APP_FUNC_PULSE_CLEAR_RESP 		be16toword(0x0041)
//--------------------------------------------------
#define DAS_ANALOG_MODE_OFF 	0x00
#define DAS_ANALOG_MODE_VOLT 	0x01
#define DAS_ANALOG_MODE_CURR 	0x02
#define DAS_ANALOG_MODE_ALL 	0x03
//--------------------------------------------------
#define DAS_PULSE_MODE_OFF 			0x00
#define DAS_PULSE_MODE_COUNTER 		0x01
#define DAS_PULSE_MODE_FREQUENCY 	0x02
//--------------------------------------------------
#define	DAS_OK				0x00
#define DAS_ERROR			0x01
#define DAS_ERROR_CHANNEL 	0x02
#define DAS_ERROR_MODE 		0x03
//--------------------------------------------------
#define DAS_MAX_PACKET_SIZE 32
#define DAS_MAX_PACKETS 	10
//--------------------------------------------------
typedef struct __DAS_InitTypeDef
{
	uint8_t AnalogChannelsCount;
	uint8_t PulseChannelsCount;
} DAS_InitTypeDef;
//--------------------------------------------------
//--------------------------------------------------
void DAS_Init(void);
void das_response(void);
uint16_t das_read(uint8_t* data, uint16_t len_data);
//--------------------------------------------------
uint8_t DAS_SetChannelMode(uint8_t ch_id, uint8_t ch_mode);
uint8_t DAS_SetAnalogChannelValue(uint8_t ch_id, float ch_value);
//--------------------------------------------------
#endif
