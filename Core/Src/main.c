/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "das.h"
#include "tcpdas.h"
#include "datareaddas.h"
#include "connection_pipeline.h"
#include "tcpecho.h"
#include "opt.h"
#include "api.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "lwip/sockets.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

void ConnectionListenTask(void *argument);
void StartServerTask1(void *argument);
void StartNetTask1(void *argument);
void ConnectionsCounter(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  // serverTaskHandle = osThreadNew(ConnectionListenTask, NULL, &serverTask_attributes);
  // serverTaskHandle = osThreadNew(ConnectionsCounter, NULL, &serverTask_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LedRed_Pin|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LedGreen_Pin|LedBlue_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LedRed_Pin PA10 */
  GPIO_InitStruct.Pin = LedRed_Pin|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LedGreen_Pin LedBlue_Pin */
  GPIO_InitStruct.Pin = LedGreen_Pin|LedBlue_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

typedef struct struct_sock_t {
  uint16_t y_pos;
  uint16_t port;
} struct_sock;

struct_sock sock01, sock02;

typedef struct struct_client_socket_t {
  struct sockaddr_in remotehost;
  socklen_t sockaddrsize;
  int accept_sock;
  uint16_t y_pos;
} struct_client_socket;

struct_client_socket client_socket01;

uint32_t value[4];

void StartNetTask(void *argument)
{
//	struct netconn *conn, *newconn;
//	struct netbuf *buf;
//	volatile err_t err;
//	struct_sock *arg_sock;
//	arg_sock = (struct_sock*) argument;
//	conn = arg_sock->conn;
//
//	u16_t len;
//	void *data;
//
//	for(;;)
//	{
//		if((err = netconn_accept(conn, &newconn)) == ERR_OK)
//		{
//			while((err = netconn_recv(newconn, &buf)) == ERR_OK)
//			{
//				do
//				{
//					  netbuf_data(buf, &data, &len);
//					  uint16_t das_len = das_read(data, len);
//
//					  if(das_len > 0)
//					  {
//						  HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
//						  netconn_write(newconn, data, das_len, NETCONN_COPY);
//					  }
//					  else
//					  {
//						  HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_SET);
//						  netconn_write(newconn, data, len, NETCONN_COPY);
//					  }
//
//				}
//				while (netbuf_next(buf) >= 0);
//
//				netbuf_delete(buf);
//
//				HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_RESET);
//				HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_RESET);
//			}
//
//			netconn_close(newconn);
//			netconn_delete(newconn);
//		}
//
//		osDelay(1);
//	}
}

struct netconn ** connection_pool;
int next_connection = 0;
int connections = 0;

// create here net lestenter task
void ConnectionListenTask(void *argument)
{
	struct netconn *conn; // , *newconn;
	volatile err_t err;
	int active_connections = 0;
	int thread_count = 0;
	char buffer [11];
	struct netbuf *buf;

	u16_t len;
	void *data;

	// initialize connection pool
	connection_pool = malloc(10 * sizeof(struct netconn *));

	// LWIP_UNUSED_ARG(argument);

	// waiting for connection
	conn = netconn_new(NETCONN_TCP);

	if(conn != NULL)
	{
		if((err = netconn_bind(conn, NULL, 8082)) == ERR_OK)
		{
			// listen for new connection
			netconn_listen(conn);

			while(1)
			{
				// create new connection
				struct netconn * newconn = malloc(sizeof(struct netconn *));
				if((err = netconn_accept(conn, &newconn)) == ERR_OK)
				{
					// serverTaskHandle = osThreadNew(StartNetTask1, (void*)&newconn, &serverTask_attributes);

//					while((err = netconn_recv(newconn, &buf)) == ERR_OK)
//					{
//						do
//						{
//							HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
////							netbuf_data(buf, &data, &len);
////							netconn_write(newconn, data, len, NETCONN_COPY);
//
//
//							// read das data
//						  uint16_t das_len = das_read(data, len);
//
//						  if(das_len > 0)
//						  {
//							  HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
//							  netconn_write(newconn, data, das_len, NETCONN_COPY);
//						  }
//						  else
//						  {
//							  HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_SET);
//							  netconn_write(newconn, data, len, NETCONN_COPY);
//						  }
//						}
//						while (netbuf_next(buf) >= 0);
//
//						HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_RESET);
//						HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_RESET);
//
//						netbuf_delete(buf);
//
//						osDelay(1);
//					}


//					HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
//					struct_sock socket;
//					socket.conn = newconn;
//					osThreadNew(StartNetTask1, (void*)&socket, &serverTask_attributes);
				}

				osDelay(1);
			}
		}
		else
		{
			netconn_delete(conn);
		}
	}
}

void ConnectionsCounter(void *argument)
{
	for(;;)
	{
		osDelay(10000);
		// HAL_NVIC_SystemReset();
	}
}

void StartNetTask1(void *argument)
{
	struct netconn * newconn = (struct netconn *)argument;
	volatile err_t err;
	struct netbuf *buf;

	u16_t len;
	void *data;

	while((err = netconn_recv(newconn, &buf)) == ERR_OK)
	{
		do
		{
		  netbuf_data(buf, &data, &len);
		  uint16_t das_len = das_read(data, len);

		  HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);

		  if(das_len > 0)
		  {
			  HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
			  netconn_write(newconn, data, das_len, NETCONN_COPY);
		  }
		  else
		  {
			  HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_SET);
			  netconn_write(newconn, data, len, NETCONN_COPY);
		  }

		}
		while (netbuf_next(buf) >= 0);

		HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_RESET);
		// HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_RESET);

		netbuf_delete(buf);
	}

	netconn_close(newconn);
	netconn_delete(newconn);

	for(;;)
	{
		osThreadTerminate(NULL);
		osDelay(1);
	}
}

unsigned char *out_buffer;
char buffer [150];

static void client_socket_thread(void *arg)
{
	int buflen = 150;
	int ret, accept_sock;
	struct sockaddr_in remotehost;
	socklen_t sockaddrsize;
	struct_client_socket  *arg_client_socket;
	arg_client_socket = (struct_client_socket *) arg;
	uint16_t das_len;

	remotehost = arg_client_socket->remotehost;
	sockaddrsize = arg_client_socket->sockaddrsize;
	accept_sock = arg_client_socket->accept_sock;

	for(;;)
	{
		ret = recvfrom(accept_sock, buffer, buflen, 0, (struct sockaddr *)&remotehost, &sockaddrsize);
		if(ret > 0)
		{
			// HAL_GPIO_WritePin(GPIOB, LedGreen_Pin, GPIO_PIN_SET);
			out_buffer[ret] = 0;

			if(strcmp((char*)out_buffer, "-c") == 0)
			{
				// HAL_GPIO_WritePin(GPIOB, LedGreen_Pin, GPIO_PIN_RESET);
				close(accept_sock);
				osThreadTerminate(NULL);
			}

//			if(strcmp((char*)out_buffer, "\r\n") != 0)
//			{
//				strcat((char*)out_buffer,"\r\n");
//				out_buffer[ret] = 0;
//				strcat((char*)out_buffer,"\r\n");
//				sendto(accept_sock, buffer, ret, 0, (struct sockaddr *)&remotehost, sockaddrsize);
//			}
//			else
//			{
//				das_len = sprintf(buffer, "Hello, world!%d", xPortGetMinimumEverFreeHeapSize());
//				sendto(accept_sock, buffer, ret, 0, (struct sockaddr *)&remotehost, sockaddrsize);
//			}
//
//			das_len = sprintf(buffer, "Hello, world!%d", xPortGetMinimumEverFreeHeapSize());
//			sendto(accept_sock, buffer, das_len, 0, (struct sockaddr *)&remotehost, sockaddrsize);

			// sendto(accept_sock, buffer, ret, 0, (struct sockaddr *)&remotehost, sockaddrsize);
//
			// das_len = das_read(buffer, ret);
//
//			HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
//
			if(das_len > 0)
			{
			  HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_SET);
			  sendto(accept_sock, buffer, das_len, 0, (struct sockaddr *)&remotehost, sockaddrsize);
			}
			else
			{
			  HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_SET);
			  sendto(accept_sock, buffer, ret, 0, (struct sockaddr *)&remotehost, sockaddrsize);
			}

			close(accept_sock);
			osThreadTerminate(NULL);
//
//			HAL_GPIO_WritePin(GPIOB, LedBlue_Pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOA, LedRed_Pin, GPIO_PIN_RESET);
		}
	}
}

static void tcp_thread(void *arg)
{
	struct_sock *arg_sock;
	int sock, accept_sock;
	struct sockaddr_in address, remotehost;
	socklen_t sockaddrsize;
	arg_sock = (struct_sock*) arg;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
	{
		address.sin_family = AF_INET;
		address.sin_port = htons(8082);
		address.sin_addr.s_addr = INADDR_ANY;

		if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == 0)
		{
			// HAL_GPIO_WritePin(GPIOB, LedGreen_Pin, GPIO_PIN_SET);
			listen(sock, 5);
			for(;;)
			{
				accept_sock = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&sockaddrsize);
				if(accept_sock >= 0)
				{
					client_socket01.accept_sock = accept_sock;
					client_socket01.remotehost = remotehost;
					client_socket01.sockaddrsize = sockaddrsize;
					client_socket01.y_pos = arg_sock->y_pos;
					sys_thread_new("client_socket_thread", client_socket_thread, (void*)&client_socket01, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);

					close(sock);
					// HAL_GPIO_WritePin(GPIOB, LedGreen_Pin, GPIO_PIN_RESET);
					osThreadTerminate(NULL);
				}
			}
		}
		else
		{
			close(sock);
			return;
		}
	}
}

static void myrg() {
	for(;;) {
		HAL_GPIO_TogglePin(GPIOB, LedGreen_Pin);
		osDelay(250);
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */

	// Init DAS
	DAS_Init();


	sock01.port = 8082;
	sock01.y_pos = 60;
	sys_thread_new("tcp_thread", tcp_thread, (void*)&sock01, DEFAULT_THREAD_STACKSIZE * 2, osPriorityNormal);

	sys_thread_new("myrg", myrg, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityRealtime7);

    // prepare ADC to read data
    // HAL_ADC_Start_DMA(&hadc1, value, 4);

  /* Infinite loop */
	for(;;)
	{
		osThreadTerminate(NULL);
//		HAL_ADC_Start_DMA(&hadc1, value, 4);
//		DAS_SetAnalogChannelValue(0, value[0]);
//		DAS_SetAnalogChannelValue(1, value[1]);
//		DAS_SetAnalogChannelValue(2, value[2]);
//		DAS_SetAnalogChannelValue(3, value[3]);

		osDelay(1);
	}

  /* USER CODE END 5 */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
