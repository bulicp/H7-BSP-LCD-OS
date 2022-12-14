/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include <math.h>
//#include "arm_math.h"

#ifdef __USE_EXTERNAL_FLASH__
#include "dashboard.h"
#endif
//#include "stlogo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */




/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define __MEASURE__
#ifdef __MEASURE__
#define __DELAY 5
#else
#define __DELAY 100
#endif

#define __SDRAM_WRITE_COMPARE___

#define OBD2DELAY 90
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;
TS_Init_t hTouchScreen;
TS_State_t  TouchScreen_State;
uint8_t touch_index = 0;


/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
__IO uint32_t ButtonState = 0;
uint32_t x_size;
uint32_t y_size;
uint16_t timer_val_start, timer_val_end;
uint16_t elapsed_rows, elapsed_cols, elapsed_dma;
volatile uint8_t mdma_complete = 0;  // nastavlja MDMA Handler
OBD2_Supported_PIDs_TypeDef supportedPIDS;
uint8_t* obd_response[20];


UART_HandleTypeDef 				UART3Handle;
DMA_HandleTypeDef    			DMA2_SDRAM_Handle;
extern SDRAM_HandleTypeDef 		sdramHand;
TIM_HandleTypeDef    			TIM3Handle;
extern LTDC_HandleTypeDef  		hlcd_ltdc;
uint8_t activeLayer = LTDC_LAYER_1;
uint8_t screen = 0;


osThreadId_t Task1Handle;
const osThreadAttr_t Task1_attributes = {
  .name = "Task 1",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task 2",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osMessageQueueId_t mid_MsgQueue;                // message queue id

uint8_t pid;
float fvalue = 0.0;
uint32_t ui32value = 0;
uint32_t saved_speed = 0;
uint32_t saved_rpms = 0;
uint32_t DashColor;



extern ALIGN_32BYTES (uint32_t sdram_read_Buffer[SDRAM_BUFFER_SIZE]);



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC3_Init(void);
void StartDefaultTask(void *argument);
static void Display_DemoDescription(void);


/* USER CODE BEGIN PFP */
static void Display_InitialContent(void);

static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
void Task1(void *argument);
void Task2(void *argument);
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
  uint8_t time_str1[60];
  uint8_t time_str2[40];

  /* Configure the MPU attributes as Write Through */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* STM32H7xx HAL library initialization:
         - Systick timer is configured by default as source of time base, but user
           can eventually implement his proper time base source (a general purpose
           timer for example or other time source), keeping in mind that Time base
           duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
           handled in milliseconds basis.
         - Set NVIC Group Priority to 4
         - Low Level Initialization
  */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();

#ifdef __MINIMAL_EXAMPLE
  /* USER CODE BEGIN SysInit */
  /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /* -2- Configure IO in output push-pull mode to drive external LEDs */
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /* -3- Toggle IO in an infinite loop */
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_2);
    /* Insert delay 100 ms */
    HAL_Delay(100);
  }
#endif

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  //MX_ADC3_Init();

  /* USER CODE BEGIN 2 */
  /* When system initialization is finished, Cortex-M7 could wakeup (when needed) the Cortex-M4  by means of
        HSEM notification or by any D2 wakeup source (SEV,EXTI..)   */

  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  BSP_LED_Off(LED_GREEN);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
/*
  while (1)
  {
    BSP_LED_On(LED_GREEN);
    BSP_LED_Off(LED_RED);
    HAL_Delay(200);
    BSP_LED_Off(LED_GREEN);
    BSP_LED_On(LED_RED);
    HAL_Delay(200);
  }
*/

  /* Configure the User push-button in EXTI Mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);

  /* Configure TIM3 timebase */
  Init_TIM3(&TIM3Handle);


  /*
  // Configure DMA2 for SDRAM:
  if (DMA2_SDRAM_Config(&DMA2_SDRAM_Handle) != HAL_OK) {
	  Error_Handler();
  }
  */

  // Configure DMA2 for SDRAM:
  if (MDMA_SDRAM_Config() != HAL_OK) {
	  Error_Handler();
  }

  /* Init UART3*/
  if (USART3_Init(&UART3Handle) != HAL_OK){
	  Error_Handler();
  }
  RetargetInit(&UART3Handle);

  // Configure DMA1 for UART3:
  if (DMA1_UART3_Config() != HAL_OK) {
	  Error_Handler();
  }

  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  //Display_InitialContent();
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, ENABLE);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, DISABLE);

  UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)dash);
  UTIL_LCD_GetPixel(120, 136, &DashColor);



  Display_InitialContent_CAN_Layer1();
  Display_InitialContent_CAN_Layer2();

  UTIL_LCD_SetLayer(LTDC_LAYER_1);
  BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, ENABLE);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, DISABLE);


  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  hTouchScreen.Width = x_size;
  hTouchScreen.Height = y_size;
  hTouchScreen.Orientation =TS_SWAP_XY ;
  hTouchScreen.Accuracy = 100;
  if (BSP_TS_Init(0, &hTouchScreen) != BSP_ERROR_NONE) {
	 Error_Handler();
  }

  /*
  if (BSP_TS_EnableIT(0) != BSP_ERROR_NONE) {
	 Error_Handler();
  }
  */

/*

  Init_SDRAM();
  //simple_SDRAM_test();
  SDRAM_init_matrices();
  //Display_InitialContent();

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
  UTIL_LCD_SetFont(&Font16);
  sprintf((char* )time_str1, (const char*)"Vsebina v SDRAM: ");
  strcat((char* )time_str1, (char *)sdram_read_Buffer);
  //UTIL_LCD_DisplayStringAt(30, (y_size/2 + 65), (uint8_t *)time_str1, LEFT_MODE);

*/

/*
#ifdef __SDRAM_WRITE_COMPARE___
  SDRAM_write_matrix();
#endif
  while (1){

	  // Row-major order access:
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_mat_row_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_rows = timer_val_end - timer_val_start;
	  else
		  elapsed_rows = timer_val_end + (65536-timer_val_start);
#ifdef __SDRAM_WRITE_COMPARE___
	  if (SDRAM_compare_matrices()==0) {
#endif
		  BSP_LED_On(LED_GREEN);
	  	  HAL_Delay(__DELAY);
#ifdef __SDRAM_WRITE_COMPARE___
	  }
	  else Error_Handler();
#endif
	  BSP_LED_Off(LED_GREEN);

	  // Column-major order access:
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_mat_col_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_cols = timer_val_end - timer_val_start;
	  else
		  elapsed_cols = timer_val_end + (65536-timer_val_start);
#ifdef __SDRAM_WRITE_COMPARE___
	  if (SDRAM_compare_matrices()==0) {
#endif
		  BSP_LED_On(LED_RED);
	  	  HAL_Delay(__DELAY);
#ifdef __SDRAM_WRITE_COMPARE___
	  }
	  else Error_Handler();
#endif
	  BSP_LED_Off(LED_RED);

	  // DMA memory transfer:
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_MDMA_mat_row_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_dma = timer_val_end - timer_val_start;
	  else
		  elapsed_dma = timer_val_end + (65536-timer_val_start);
#ifdef __SDRAM_WRITE_COMPARE___
	  if (SDRAM_compare_matrices()==0) {
#endif
		  BSP_LED_On(LED_RED);
		  BSP_LED_On(LED_GREEN);
	  	  HAL_Delay(__DELAY);
	  	  BSP_LED_On(LED_RED);
		  BSP_LED_On(LED_GREEN);
	  	  HAL_Delay(__DELAY);
#ifdef __SDRAM_WRITE_COMPARE___
	  }
	  else Error_Handler();
#endif
	  BSP_LED_Off(LED_RED);
	  BSP_LED_Off(LED_GREEN);


	  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
	  sprintf((char*)time_str1, "Rows:%5dus  Cols:%5dus MDMA:%4dus", elapsed_rows, elapsed_cols, elapsed_dma);
	  UTIL_LCD_DisplayStringAt(0, (y_size/2 + 25), (uint8_t *)time_str1, CENTER_MODE);

	  //strcat((char*)time_str1, "  DMA\n");
      //HAL_UART_Transmit_DMA(&UART3Handle, (uint8_t *)time_str1, strlen((uint8_t *)time_str1));
	  printf("%s \n", time_str1);
	  HAL_Delay(__DELAY);

  }

*/


  if (FDCAN1_Config() != HAL_OK) {
	 Error_Handler();
  }


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
  mid_MsgQueue = osMessageQueueNew(16, sizeof(CAN_OBD2_MSGQUEUE_OBJ_t), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Task1Handle = osThreadNew(Task1, NULL, &Task1_attributes);
  Task2Handle = osThreadNew(Task2, NULL, &Task2_attributes);
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
	  __NOP();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}




/**
  * @brief  Display main demo messages
  * @param  None
  * @retval None
  */
static void Display_InitialContent(void)
{

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  UTIL_LCD_SetFont(&Font16);

  /* Clear the LCD */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  BSP_LCD_FillRect(0, 0, 0, x_size, y_size, UTIL_LCD_COLOR_BLACK);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 20, (uint8_t *)"STM32H750B BSP", CENTER_MODE);
  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"Organizacija racunalniskih", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 70, (uint8_t *)"sistemov", CENTER_MODE);


  /* Draw Bitmap */
  //UTIL_LCD_DrawBitmap((x_size - 80)/2, 65, (uint8_t *)stlogo);

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, y_size - 20, (uint8_t *)"Copyright (c) Pa3cio 2022", CENTER_MODE);

  UTIL_LCD_SetFont(&Font20);
  BSP_LCD_FillRect(0, 0, y_size/2 - 35, x_size, 120, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size/2 - 25 , (uint8_t *)"FMC SDRAM test", CENTER_MODE);

}



/**
  * @brief  Check for user input
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  return ButtonState;
}
/**
* @brief  EXTI line detection callbacks.
* @param  GPIO_Pin: Specifies the pins connected EXTI line
* @retval None
*/
void BSP_PB_Callback(Button_TypeDef Button)
{
 if(Button == BUTTON_USER)
  {
    ButtonState = 1;
  }
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 20
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 20;  /* fdcan_ker_ck = 40 MHz */


  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

 /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
 */

  /*activate CSI clock mondatory for I/O Compensation Cell*/
  __HAL_RCC_CSI_ENABLE() ;

  /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;

  /* Enables the I/O Compensation Cell */
  HAL_EnableCompensationCell();
}



/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef _MX_INIT
/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_DIFFERENTIAL_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

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
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PI6 PI5 PI4 PI7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_SAI2;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : FDCAN2_RX_Pin FDCAN2_TX_Pin */
  GPIO_InitStruct.Pin = FDCAN2_RX_Pin|FDCAN2_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PK5 PK4 PK6 PK3
                           PK7 PK2 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_3
                          |GPIO_PIN_7|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : PG10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_SAI2;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : SDIO1_D2_Pin SDIO1_D3_Pin SDIO1_CK_Pin SDIO1_D0_Pin
                           SDIO1_D1_Pin SDIO1_D7_Pin SDIO1_D6_Pin */
  GPIO_InitStruct.Pin = SDIO1_D2_Pin|SDIO1_D3_Pin|SDIO1_CK_Pin|SDIO1_D0_Pin
                          |SDIO1_D1_Pin|SDIO1_D7_Pin|SDIO1_D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PI1 PI0 PI9 PI12
                           PI14 PI15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_9|GPIO_PIN_12
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PE1 PE0 PE10 PE9
                           PE11 PE12 PE15 PE8
                           PE13 PE7 PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_9
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_13|GPIO_PIN_7|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MII_TX_EN_Pin MII_TXD1_Pin MII_TXD0_Pin */
  GPIO_InitStruct.Pin = MII_TX_EN_Pin|MII_TXD1_Pin|MII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DISP_Pin PJ14 PJ12 PJ13
                           PJ11 PJ10 PJ9 PJ0
                           PJ8 PJ7 PJ6 PJ1
                           PJ5 PJ3 PJ4 */
  GPIO_InitStruct.Pin = LCD_DISP_Pin|GPIO_PIN_14|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_0
                          |GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_1
                          |GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pin : PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PI2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PH15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : FDCAN1_RX_Pin FDCAN1_TX_Pin */
  GPIO_InitStruct.Pin = FDCAN1_RX_Pin|FDCAN1_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : MII_TXD3_Pin */
  GPIO_InitStruct.Pin = MII_TXD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(MII_TXD3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DISPD7_Pin */
  GPIO_InitStruct.Pin = LCD_DISPD7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_DISPD7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PE5 PE4 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_SAI4;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : SDIO1_D5_Pin SDIO1_D4_Pin */
  GPIO_InitStruct.Pin = SDIO1_D5_Pin|SDIO1_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG15 PG8 PG5 PG4
                           PG0 PG1 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_5|GPIO_PIN_4
                          |GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : SDIO1_CMD_Pin */
  GPIO_InitStruct.Pin = SDIO1_CMD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(SDIO1_CMD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD15 PD14
                           PD10 PD9 PD8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_14
                          |GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS2_ID_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS2_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS2_ID_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_OTG_FS2_P_Pin USB_OTG_FS2_N_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS2_P_Pin|USB_OTG_FS2_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : MII_RX_ER_Pin */
  GPIO_InitStruct.Pin = MII_RX_ER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(MII_RX_ER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PF2 PF1 PF0 PF3
                           PF5 PF4 PF13 PF14
                           PF12 PF15 PF11 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
                          |GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_INT_Pin */
  GPIO_InitStruct.Pin = LCD_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PF6 PF7 PF10 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : MII_MDC_Pin MII_TXD2_Pin MII_TX_CLK_Pin MII_RXD0_Pin
                           MII_RXD1_Pin */
  GPIO_InitStruct.Pin = MII_MDC_Pin|MII_TXD2_Pin|MII_TX_CLK_Pin|MII_RXD0_Pin
                          |MII_RXD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MII_CRS_Pin MII_COL_Pin */
  GPIO_InitStruct.Pin = MII_CRS_Pin|MII_COL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : MII_MDIO_Pin MII_RX_CLK_Pin MII_RX_DV_Pin */
  GPIO_InitStruct.Pin = MII_MDIO_Pin|MII_RX_CLK_Pin|MII_RX_DV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PH5 PH6 PH7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : VCP_TX_Pin VCP_RX_Pin */
  GPIO_InitStruct.Pin = VCP_TX_Pin|VCP_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS2_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS2_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS2_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PH9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : MII_RX_D3_Pin MII_RX_D2_Pin */
  GPIO_InitStruct.Pin = MII_RX_D3_Pin|MII_RX_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LD1_Pin */
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);


  /*Configure GPIO pin : LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_OPEN);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_OPEN);
}

#endif


/* USER CODE BEGIN 4 */
void Task1(void *argument)
{
  /* USER CODE BEGIN 5 */
  CAN_OBD2_MSGQUEUE_OBJ_t queue_data;
  osStatus_t status = osOK;


  /* Infinite loop */
  for(;;)
  {
	osDelay(100);
	BSP_LCD_GetXSize(0, &x_size);
	UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_SetFont(&Font20);
    memset(obd_response, "\0", sizeof(obd_response));


	status = osMessageQueueGet(mid_MsgQueue, &queue_data, NULL, osWaitForever);
    if (status == osOK){

    	pid = queue_data.pid;

    	if (touch_index < 2){

			switch (pid){
			case OBD2_PID_PIDS_SUPPORTED_01_20:
				//OBD2DecodeSupportedPIDs(queue_data.OBDData, &supportedPIDS);
				ui32value = ((uint32_t)queue_data.OBDData[0]) << 24;
				ui32value |= ((uint32_t)queue_data.OBDData[1]) << 16;
				ui32value |= ((uint32_t)queue_data.OBDData[2]) << 8;
				ui32value |= ((uint32_t)queue_data.OBDData[3]);

				sprintf(obd_response, "Supported PIDS: %x", ui32value);
				//UTIL_LCD_DisplayStringAt(0, y_size - 20, obd_response, CENTER_MODE);
				break;
			case OBD2_PID_PIDS_SUPPORTED_21_40:
				ui32value = ((uint32_t)queue_data.OBDData[0]) << 24;
				ui32value |= ((uint32_t)queue_data.OBDData[1]) << 16;
				ui32value |= ((uint32_t)queue_data.OBDData[2]) << 8;
				ui32value |= ((uint32_t)queue_data.OBDData[3]);
				sprintf(obd_response, "Supported PIDS: %x", ui32value);

				break;
			case OBD2_PID_PIDS_SUPPORTED_41_60:
				ui32value = ((uint32_t)queue_data.OBDData[0]) << 24;
				ui32value |= ((uint32_t)queue_data.OBDData[1]) << 16;
				ui32value |= ((uint32_t)queue_data.OBDData[2]) << 8;
				ui32value |= ((uint32_t)queue_data.OBDData[3]);
				sprintf(obd_response, "Supported PIDS: %x", ui32value);

				break;
			case OBD2_PID_PIDS_SUPPORTED_61_80:
				ui32value = ((uint32_t)queue_data.OBDData[0]) << 24;
				ui32value |= ((uint32_t)queue_data.OBDData[1]) << 16;
				ui32value |= ((uint32_t)queue_data.OBDData[2]) << 8;
				ui32value |= ((uint32_t)queue_data.OBDData[3]);
				sprintf(obd_response, "Supported PIDS: %x", ui32value);

				break;

			// ************************************************************************
			// ROW 1:
			case OBD2_PID_VEHICLE_SPEED:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeVehicleSpeed(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d km/h", ui32value);
				UTIL_LCD_DisplayStringAt(30, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_ENGINE_SPEED:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeEngineSpeed(queue_data.OBDData);
				sprintf((char*)obd_response, "%5d", ui32value);
				UTIL_LCD_DisplayStringAt(200, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_ENGINE_LOAD:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				fvalue = OBD2DecodeEngineLoad(queue_data.OBDData);
				sprintf((char*)obd_response, "%2d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(380, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;

			case OBD2_PID_FUEL_RAIL_GAUGE_PRESSURE:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeFuelRailPressure(queue_data.OBDData);
				sprintf((char*)obd_response, "%6d kPa", ui32value);
				UTIL_LCD_DisplayStringAt(10, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_DISTANCE_SINCE_CODES_CLEARED:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeDistanceSinceCodeCleared(queue_data.OBDData);
				sprintf((char*)obd_response, "%6d km", ui32value);
				UTIL_LCD_DisplayStringAt(180, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_ABS_BARO_PRESSURE:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeAbsoluteBarometricPressure(queue_data.OBDData);
				sprintf((char*)obd_response, "%4d hPa", ui32value);
				UTIL_LCD_DisplayStringAt(350, YROW1 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;



			// ************************************************************************
			// Row 2:
			case OBD2_PID_ENGINE_COOLANT_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeEngineCoolantTemp(queue_data.OBDData);
				sprintf(obd_response, "%3d deg", ui32value);
				UTIL_LCD_DisplayStringAt(35, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_INTAKE_AIR_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeIntakeAirTemp(queue_data.OBDData);
				sprintf(obd_response, "%d deg", ui32value);
				UTIL_LCD_DisplayStringAt(195, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_FUEL_SYS_STATUS:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				OBD2DecodeFuelSystemStatus(queue_data.OBDData, obd_response, sizeof(obd_response));
				UTIL_LCD_SetFont(&Font12);
				UTIL_LCD_DisplayStringAt(360, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				UTIL_LCD_SetFont(&Font20);
				break;

			case OBD2_PID_ABS_CATALYST_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeCatalystTemperature(queue_data.OBDData);
				sprintf(obd_response, "%3d deg", (uint32_t)fvalue);
				UTIL_LCD_DisplayStringAt(35, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_CONTROL_MODULE_VOLTAGE:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeControlModuleVoltage(queue_data.OBDData);
				sprintf(obd_response, "%d V", (uint32_t)fvalue);
				UTIL_LCD_DisplayStringAt(215, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_AMBIENT_AIR_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeAmbientAirTemperature(queue_data.OBDData);
				sprintf(obd_response, "%2d deg", ui32value);
				UTIL_LCD_DisplayStringAt(345, YROW2 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;

			// ************************************************************************
			// Row 3:
			case OBD2_PID_OXY_SENSOR_1:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeOxySensor1(queue_data.OBDData);
				sprintf(obd_response, "%4d mV", ui32value);
				UTIL_LCD_DisplayStringAt(20, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_OXY_SENSOR_2:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeOxySensor2(queue_data.OBDData);
				sprintf(obd_response, "%4d mV", ui32value);
				UTIL_LCD_DisplayStringAt(195, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_THROTTLE_POSITION:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				fvalue = OBD2DecodeThrottlePosition(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(370, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;

			case OBD2_PID_ACCELERATOR_PEDAL_POSITION_D:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeAcceleratedPedalPosition(queue_data.OBDData);
				sprintf(obd_response, "%3d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(30, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_ACCELERATOR_PEDAL_POSITION_E:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeAcceleratedPedalPosition(queue_data.OBDData);
				sprintf(obd_response, "%3d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(205, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_ENGINE_FUEL_INJECTION_TIMING:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeFuelInjectionTiming(queue_data.OBDData);
				sprintf((char*)obd_response, "%5d deg", ui32value);
				UTIL_LCD_DisplayStringAt(340, YROW3 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;


			// ************************************************************************
			// Row 4:
			case OBD2_PID_MAF_FLOW_RATE:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				fvalue = OBD2DecodeMAFRate(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d g/s", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(25, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_RUN_SINCE_ENGINE_START:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeRunTime(queue_data.OBDData);
				sprintf((char*)obd_response, "%5d s", ui32value);
				UTIL_LCD_DisplayStringAt(190, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_INTAKE_MANIFOLD_PRESSURE:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeManifoldPressure(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d kPa", ui32value);
				UTIL_LCD_DisplayStringAt(350, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;


			case OBD2_PID_COMMANDED_THROTTLE_ACTUATOR:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeCommandedThrottlePosition(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(25, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_COMMANDED_EGR_ERROR:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				ui32value = OBD2DecodeCommandedEGRError(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d %%", ui32value);
				UTIL_LCD_DisplayStringAt(200, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;
			case OBD2_PID_COMMANDED_EGR:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
				fvalue = OBD2DecodeCommandedEGR(queue_data.OBDData);
				sprintf((char*)obd_response, "%3d %%", (int32_t)fvalue);
				UTIL_LCD_DisplayStringAt(350, YROW4 + TEXTOFFSET, obd_response, LEFT_MODE);
				break;


			default:
				break;
			}
    	}	// if touch_index < 2
    	else {		// touch_index = 2
    		switch (pid){
    		case OBD2_PID_VEHICLE_SPEED:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				// Delete previous speed line:
				DrawSpeedLine(saved_speed, DashColor);
				//UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)dash);
				// Draw new line:
				ui32value = OBD2DecodeVehicleSpeed(queue_data.OBDData);
				saved_speed = ui32value;
				DrawSpeedLine(ui32value,  UTIL_LCD_COLOR_CYAN);
				break;
			case OBD2_PID_ENGINE_SPEED:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				// Delete previous rpms line:
				DrawRPMLine(saved_rpms, DashColor);
				//UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)dash);
				// Draw new line:
				ui32value = OBD2DecodeEngineSpeed(queue_data.OBDData);
				saved_rpms = ui32value;
				DrawRPMLine(ui32value,  UTIL_LCD_COLOR_CYAN );
				break;
			case OBD2_PID_ENGINE_COOLANT_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeEngineCoolantTemp(queue_data.OBDData);
				sprintf(obd_response, "%3d", ui32value);
				UTIL_LCD_SetFont(&Font24);
				UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
				if (ui32value >= 90)
					UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
				else
					UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_YELLOW);
				UTIL_LCD_DisplayStringAt(0, 245, obd_response, CENTER_MODE);
				UTIL_LCD_SetFont(&Font20);
				UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
				UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
				break;

			case OBD2_PID_AMBIENT_AIR_TEMP:
				BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
				ui32value = OBD2DecodeAmbientAirTemperature(queue_data.OBDData);
				sprintf(obd_response, "%2d", ui32value);
				UTIL_LCD_SetFont(&Font24);
				UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
				UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_LIGHTBLUE);
				UTIL_LCD_DisplayStringAt(0, 5, obd_response, CENTER_MODE);
				UTIL_LCD_SetFont(&Font20);
				UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
				UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
			default:
				break;

    		}
    	}
    }
  }
}


void Task2(void *argument)
{
  /* USER CODE BEGIN 5 */
  CAN_OBD2_MSGQUEUE_OBJ_t queue_element;
  uint32_t size;
  osStatus_t osstatus;

  /* Infinite loop */
  for(;;)
  {

	  if (touch_index < 2){
		  /*
		  OBD2_SendQuery(0x01, OBD2_PID_PIDS_SUPPORTED_01_20);
		  osDelay(100)
		  OBD2_SendQuery(0x01, OBD2_PID_PIDS_SUPPORTED_21_40);
		  osDelay(100);
		  OBD2_SendQuery(0x01, OBD2_PID_PIDS_SUPPORTED_41_60);
		  osDelay(100);
		  OBD2_SendQuery(0x01, OBD2_PID_PIDS_SUPPORTED_61_80);
		  osDelay(100);
		  */

		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_LOAD);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_COOLANT_TEMP);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_INTAKE_AIR_TEMP);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_FUEL_SYS_STATUS);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_RUN_SINCE_ENGINE_START);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_OXY_SENSOR_1);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_OXY_SENSOR_2);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_THROTTLE_POSITION);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_MAF_FLOW_RATE);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_RUN_SINCE_ENGINE_START);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_INTAKE_MANIFOLD_PRESSURE);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_FUEL_RAIL_GAUGE_PRESSURE);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_DISTANCE_SINCE_CODES_CLEARED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ABS_BARO_PRESSURE);
		  osDelay(OBD2DELAY);



		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_RUN_SINCE_ENGINE_START);
		  osDelay(OBD2DELAY);

		  OBD2_SendQuery(0x01, OBD2_PID_ABS_CATALYST_TEMP);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_CONTROL_MODULE_VOLTAGE);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_AMBIENT_AIR_TEMP);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);

		  OBD2_SendQuery(0x01, OBD2_PID_ACCELERATOR_PEDAL_POSITION_D);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ACCELERATOR_PEDAL_POSITION_E);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_FUEL_INJECTION_TIMING);
		  osDelay(OBD2DELAY);


		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_RUN_SINCE_ENGINE_START);
		  osDelay(OBD2DELAY);

		  OBD2_SendQuery(0x01, OBD2_PID_COMMANDED_THROTTLE_ACTUATOR);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_COMMANDED_EGR);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_COMMANDED_EGR_ERROR);
		  osDelay(OBD2DELAY);

	  }
	  else {
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_AMBIENT_AIR_TEMP);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		   OBD2_SendQuery(0x01, OBD2_PID_ENGINE_COOLANT_TEMP);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
		  osDelay(OBD2DELAY);
		  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
		  osDelay(OBD2DELAY);
	  }
  }

}



void BSP_TS_Callback(uint32_t Instance){

	BSP_TS_GetState(0, &TouchScreen_State);
	if(TouchScreen_State.TouchDetected)
	{
		/* One or dual touch have been detected          */
		/* Only take into account the first touch so far */

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
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(150);
    //HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
    BSP_LED_On(LED_GREEN);
    osDelay(150);
    //HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
    BSP_LED_Off(LED_GREEN);
    BSP_TS_GetState(0, &TouchScreen_State);
    if(TouchScreen_State.TouchDetected)
    {
    	/* One or dual touch have been detected          */
        /* Only take into account the first touch so far */

    	touch_index ++ ;
    	if (touch_index == 3) touch_index = 0;

    	if(touch_index == 0) {
    		BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
    		Display_InitialContent_CAN_Layer1();
    		activeLayer = LTDC_LAYER_1;
			BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, ENABLE);
			BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, DISABLE);
    	}
		else if(touch_index == 1) {
			BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
			Display_InitialContent_CAN_Layer2();
			activeLayer = LTDC_LAYER_2;
			BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, ENABLE);
			BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, DISABLE);
		}
    	else {
#ifdef __USE_EXTERNAL_FLASH__
    		BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
    		//UTIL_LCD_GetPixel(120, 136, &DashColor);
    	    activeLayer = LTDC_LAYER_1;
    		UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)dash);
    		BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, ENABLE);
			BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, DISABLE);
    		osDelay(10);
#endif
			__NOP();
    	}
    }
  }
  /* USER CODE END 5 */
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
  BSP_LED_On(LED_RED);
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
