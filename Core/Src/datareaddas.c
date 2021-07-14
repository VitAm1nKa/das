#include "datareaddas.h"

#include "opt.h"
#include "das.h"
#include "api.h"
#include "sys.h"

#define DATAREADDAS_THREAD_PRIO (tskIDLE_PRIORITY + 4)

uint32_t analogData_01 = 0;
uint32_t analogData_02 = 0;
uint32_t analogData_03 = 0;
uint32_t analogData_04 = 0;

static void datareaddas_thread(void *arg)
{
	// here read analog data

	analogData_01 += 1;
	analogData_02 += 2;
	analogData_03 += 3;
	analogData_04 += 4;

	// update DAS data
	DAS_SetAnalogChannelValue(0, analogData_01);
	DAS_SetAnalogChannelValue(1, analogData_02);
	DAS_SetAnalogChannelValue(2, analogData_03);
	DAS_SetAnalogChannelValue(3, analogData_04);

	osDelay(500);
}

void datareaddas_init(void)
{
	sys_thread_new("datareaddas_thread", datareaddas_thread, NULL, DEFAULT_THREAD_STACKSIZE, DATAREADDAS_THREAD_PRIO);
}
