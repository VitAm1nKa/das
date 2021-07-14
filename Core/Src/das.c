#include "das.h"
#include <string.h>
//--------------------------------------------------
uint8_t das_data[DAS_MAX_PACKET_SIZE];
uint8_t das_data_req[DAS_MAX_PACKET_SIZE * DAS_MAX_PACKETS];
//--------------------------------------------------
extern char 	str1[60];
//--------------------------------------------------
uint16_t slip_das(uint8_t* data, uint16_t start, uint16_t len_data);
uint16_t byte_serialize(uint8_t* data_buf, uint16_t data_start, uint8_t* dest_buf, uint16_t dest_start, uint16_t len_data);
uint16_t byte_deserialize(uint8_t* data_buf, uint8_t* dest_buf, uint16_t start, uint16_t data_len);
uint16_t das_makePacket(uint8_t* data, uint16_t start);
//--------------------------------------------------
typedef union __floatData {
	float value;
	uint8_t data[4];
} floatData;

typedef union __uint32Data {
	uint32_t value;
	uint8_t data[4];
} uint32Data;

typedef struct __analog_channel {
	uint8_t id;
	uint8_t mode;
	floatData value;
} analog_channel;

typedef struct __pulse_channel {
	uint8_t id;
	uint8_t mode;
	uint32_t count;
	float frequency;
} pulse_channel;

typedef struct das_protocol {
	uint16_t	num_of_pks;
	uint16_t	cur_pk_num;
	uint16_t	app_fn_num;
	uint16_t	data_l;
	uint8_t		data[];
} das_protocol_ptr;
typedef struct das_crc32 {
	uint32_t crc32;
} das_crc32_ptr;

analog_channel channels[4];
pulse_channel pulse_channels[2];
//--------------------------------------------------
int32_t max_requests;
//--------------------------------------------------
void DAS_Init(void)
{
	channels[0].id = 0;
	channels[0].mode = DAS_ANALOG_MODE_CURR; // DAS_ANALOG_MODE_OFF
	channels[0].value.value = 0.0;
	channels[1].id = 1;
	channels[1].mode = DAS_ANALOG_MODE_CURR; // DAS_ANALOG_MODE_OFF
	channels[1].value.value = 0.0;
	channels[2].id = 2;
	channels[2].mode = DAS_ANALOG_MODE_CURR; // DAS_ANALOG_MODE_OFF
	channels[2].value.value = 0.0;
	channels[3].id = 3;
	channels[3].mode = DAS_ANALOG_MODE_CURR; // DAS_ANALOG_MODE_OFF
	channels[3].value.value = 0.0;

	pulse_channels[0].id = 0;
	pulse_channels[0].mode = DAS_PULSE_MODE_COUNTER; // DAS_PULSE_MODE_OFF
	pulse_channels[0].frequency = 0.0f;
	pulse_channels[0].count = 0;

	pulse_channels[1].id = 1;
	pulse_channels[1].mode = DAS_PULSE_MODE_COUNTER; // DAS_PULSE_MODE_OFF
	pulse_channels[1].frequency = 0.0f;
	pulse_channels[1].count = 0;

	max_requests = 5;
}

uint8_t DAS_AnalogChannelExists(uint8_t ch_id)
{
	return ch_id > 3 ? 0 : 1;
}

uint8_t DAS_PulseChannelExists(uint8_t ch_id)
{
	return ch_id > 1 ? 0 : 1;
}

uint8_t DAS_IsPulseChannelModeExists(uint8_t ch_mode)
{
	return ch_mode > 2 ? 0 : 1;
}

uint8_t DAS_SetChannelMode(uint8_t ch_id, uint8_t ch_mode)
{
	if(!DAS_AnalogChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(ch_mode > 2)
	{
		return DAS_ERROR_MODE;
	}

	channels[ch_id].mode = ch_mode;

	return DAS_OK;
}
uint8_t DAS_SetAnalogChannelValue(uint8_t ch_id, float ch_value)
{
	if(!DAS_AnalogChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(channels[ch_id].mode == DAS_ANALOG_MODE_OFF)
	{
		return DAS_ERROR_MODE;
	}

	channels[ch_id].value.value = ch_value;

	return DAS_OK;
}
uint8_t DAS_GetAnalogChannelValueData(uint8_t ch_id, uint8_t *ch_value)
{
	if(!DAS_AnalogChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(channels[ch_id].mode == DAS_ANALOG_MODE_OFF)
	{
		return DAS_ERROR_MODE;
	}

	ch_value = channels[ch_id].value.data;

	return DAS_OK;
}
uint8_t DAS_SetPulseChannelMode(uint8_t ch_id, uint8_t ch_mode)
{
	if(!DAS_PulseChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(!DAS_IsPulseChannelModeExists(ch_mode))
	{
		return DAS_ERROR_MODE;
	}

	pulse_channels[ch_id].mode = ch_mode;

	return DAS_OK;
}
uint8_t DAS_SetPulseChannelValue(uint8_t ch_id, uint32_t ch_value)
{
	if(!DAS_PulseChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(pulse_channels[ch_id].mode == DAS_PULSE_MODE_OFF)
	{
		return DAS_ERROR_MODE;
	}

	// TODO:
	// pulse_channels[ch_id].value = ch_value;

	return DAS_OK;
}
uint8_t DAS_GetPulseChannelValue(uint8_t ch_id, uint32_t *ch_value)
{
	if(!DAS_PulseChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	if(pulse_channels[ch_id].mode == DAS_PULSE_MODE_OFF)
	{
		return DAS_ERROR_MODE;
	}

	// TODO:
	// *ch_value = pulse_channels[ch_id].value;

	return DAS_OK;
}
uint8_t DAS_ResetPulseChannel(uint8_t ch_id)
{
	if(!DAS_PulseChannelExists(ch_id))
	{
		return DAS_ERROR_CHANNEL;
	}

	pulse_channels[ch_id].count = 0;

	return DAS_OK;
}
//--------------------------------------------------
static void das_print(uint8_t* buf, uint16_t start, uint16_t buf_len, char *pref){}

uint8_t counter = 0;

uint16_t das_read(uint8_t* data, uint16_t len_data)
{
	uint16_t txPointer = 0;

	if(len_data > 0)
	{
		uint16_t i;
		uint16_t len = 0;
		uint16_t das_len = 0;

		for(i = 0; i < len_data; i++)
		{
			if(i + 1 == len_data || data[i + 1] == SLIP_END)
			{
				das_len = byte_deserialize(data, das_data, len + 1, i - len);

				if(das_len >= (sizeof(das_protocol_ptr) + 4))
				{

					// das_len = slip_das(data, len + 1, len_data - len - 1);

					// Checksum verify
					das_crc32_ptr *res_cs = (void*)(das_data + das_len - 4);
					uint32_t crc = be32todword(crc32(das_data, das_len - 4, 0));
					if(res_cs->crc32 == crc)
					{
						// Print stuff
						// das_print(das_data, 0, das_len, "SER: ");

						// Process DAS message
						das_len = das_makePacket(das_data, 0);

						// Print stuff
						// das_print(das_data, 0, das_len, "PCC: ");

						// DAS Message deserialize
						das_len = byte_serialize(das_data, 0, das_data_req, txPointer, das_len);

						// Print stuff
						// das_print(das_data_req, txPointer, das_len, "DES: ");

						// Increase TX pointer
						txPointer += das_len;
					}
				}

				len = i + 1;
			}
		}
	}

	if(txPointer > 0)
	{
		// Send data
		realloc(data, txPointer);
		memcpy(data, das_data_req, txPointer);
		// das_print(data, 0, txPointer, "->");
	}

	return txPointer;
}

uint16_t das_makePacket(uint8_t* data, uint16_t start)
{
	// DAS protocol; LITTLE ENDIAN
	das_protocol_ptr *protocol = (void*)(das_data + 1);

	// Global variables
	uint8_t i;
	uint32Data u32_data;
	floatData f_data;
	uint16_t len = be16toword(protocol->data_l);

	switch(protocol->app_fn_num)
	{
		case DAS_APP_FUNC_ECHO:
			protocol->app_fn_num = DAS_APP_FUNC_ECHO_RESP;
			break;
		case DAS_APP_FUNC_ANALOG_CFG:

			// Validate data length
			if(len != 2)
			{
				protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS;
				len = 0;
			}
			else
			{
				switch(DAS_SetChannelMode(protocol->data[0], protocol->data[1]))
				{
					case DAS_ERROR_MODE:
						protocol->app_fn_num = DAS_APP_FUNC_ERROR_ARG;
						len = 0;
						break;
					case DAS_ERROR_CHANNEL:
						protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_LN_ID;
						len = 0;
						break;
					case DAS_OK:
						protocol->app_fn_num = DAS_APP_FUNC_ANALOG_CFG_RESP;
						len = 0;
						break;
				}
			}

			break;
		case DAS_APP_FUNC_READ_ANALOG:
			// Validate data length
			if(len > 1)
			{
				protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS;
				len = 0;
			}
			else if(len == 1)
			{
				if(!DAS_AnalogChannelExists(protocol->data[0]))
				{
					protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_LN_ID;
					len = 0;
				}
				else if(channels[protocol->data[0]].mode == DAS_ANALOG_MODE_OFF)
				{
					protocol->app_fn_num = DAS_APP_FUNC_ERROR_ARG;
					len = 0;
				}
				else
				{
					protocol->app_fn_num = DAS_APP_FUNC_READ_ANALOG_RESP;
					rmemcpy(protocol->data + 1, channels[protocol->data[0]].value.data, 4);
					len += 4;

				}
			}
			else
			{
				*(protocol->data) = 0;
				// All channels
				for(i = 0; i < 4; i++)
				{
					if(channels[i].mode != DAS_ANALOG_MODE_OFF)
					{
						*(protocol->data) = *(protocol->data) + 1;
						rmemcpy(protocol->data + 1 + 4 * i, channels[i].value.data, 4);
						len += 4;
					}
				}

				len++;
				protocol->app_fn_num = DAS_APP_FUNC_READ_ANALOG_RESP;
			}

			break;
		case DAS_APP_FUNC_PULSE_CFG:
			if(len != 2)
			{
				protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS;
				len = 0;
			}
			else
			{
				switch(DAS_SetPulseChannelMode(protocol->data[0], protocol->data[1]))
				{
					case DAS_ERROR_MODE:
						protocol->app_fn_num = DAS_APP_FUNC_ERROR_ARG;
						len = 0;
						break;
					case DAS_ERROR_CHANNEL:
						protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_LN_ID;
						len = 0;
						break;
					case DAS_OK:
						protocol->app_fn_num = DAS_APP_FUNC_PULSE_CFG_RESP;
						len = 0;
						break;
				}
			}
			break;
		case DAS_APP_FUNC_PULSE_READ:
			if(len != 1)
			{
				protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS;
				len = 0;
			}
			else
			{
				if(!DAS_PulseChannelExists(protocol->data[0]))
				{
					protocol->app_fn_num = DAS_APP_FUNC_ERROR_PLS_LN;
					len = 0;
				}
				else if(pulse_channels[protocol->data[0]].mode == DAS_PULSE_MODE_OFF)
				{
					protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_LN_ID;
					len = 0;
				}
				else
				{
					protocol->app_fn_num = DAS_APP_FUNC_PULSE_READ_RESP;

					if(pulse_channels[protocol->data[0]].mode == DAS_PULSE_MODE_COUNTER)
					{
						// u32_data.value = pulse_channels[protocol->data[0]].count++;
						u32_data.value = pulse_channels[protocol->data[0]].count;
						rmemcpy(protocol->data + 1, u32_data.data, 4);
						len += 4;
					}
					else if(pulse_channels[protocol->data[0]].mode == DAS_PULSE_MODE_FREQUENCY)
					{
						f_data.value = pulse_channels[protocol->data[0]].frequency;
						// f_data.value = pulse_channels[protocol->data[0]].frequency++;
						rmemcpy(protocol->data + 1, f_data.data, 4);
						len += 4;
					}
				}
			}
			break;
		case DAS_APP_FUNC_PULSE_CLEAR:
			if(len != 1)
			{
				protocol->app_fn_num = DAS_APP_FUNC_ERROR_INV_NUM_OF_ARGS;
				len = 0;
			}
			else
			{
				if(!DAS_PulseChannelExists(protocol->data[0]))
				{
					protocol->app_fn_num = DAS_APP_FUNC_ERROR_PLS_LN;
					len = 0;
				}
				else
				{
					protocol->app_fn_num = DAS_APP_FUNC_PULSE_CLEAR_RESP;
					// TODO: call function
					pulse_channels[protocol->data[0]].count = 0;
					len = 0;
				}
			}
			break;
	}

	// set header
	// set data length
	protocol->data_l = be16toword(len);

	// calc crc32
	uint32_t crc = crc32(das_data, len + 9, 0);
	das_data[len + 9] = crc >> 24 & 0xFF;
	das_data[len + 10] = crc >> 16 & 0xFF;
	das_data[len + 11] = crc >> 8 & 0xFF;
	das_data[len + 12] = crc & 0xFF;

	return len + 13;
}

uint16_t byte_serialize(uint8_t* data_buf, uint16_t data_start, uint8_t* dest_buf, uint16_t dest_start, uint16_t len_data)
{
	// das_data_req

	uint16_t i = 0;
	uint16_t ptr = dest_start;

	das_data_req[ptr++] = SLIP_END;

	for(i = data_start; i < data_start + len_data; i++)
	{
		if(data_buf[i] == SLIP_END)
		{
			dest_buf[ptr++] = SLIP_ESC;
			dest_buf[ptr++] = SLIP_ESC_END;
		}
		else if(data_buf[i] == SLIP_ESC)
		{
			dest_buf[ptr++] = SLIP_ESC;
			dest_buf[ptr++] = SLIP_ESC_ESC;
		}
		else
		{
			dest_buf[ptr++] = data_buf[i];
		}
	}

	return ptr - dest_start;
}

uint16_t byte_deserialize(uint8_t* data_buf, uint8_t* dest_buf, uint16_t start, uint16_t data_len)
{
	uint8_t i = 0;
	uint16_t len = 0;

	for(i = start; i < start + data_len; i++)
	{
		if(data_buf[i] == SLIP_ESC && data_buf[i + 1] == SLIP_ESC_END)
		{
			dest_buf[len++] = SLIP_END;
			i += 1;
		}
		else if(data_buf[i] == SLIP_ESC && data_buf[i + 1] == SLIP_ESC_ESC)
		{
			dest_buf[len++] = SLIP_ESC;
			i += 1;
		}
		else
		{
			dest_buf[len++] = data_buf[i];
		}
	}

	return len;
}
