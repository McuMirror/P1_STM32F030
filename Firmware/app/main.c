/******************************************************************************
*	Written by Yilmaz Eyidogan
*	main file of IO control unit
*	Created date: 2014.12.22
*******************************************************************************/
#include "main.h"
/*---------------------------- Symbol Define -------------------------------*/
//#define STACK_SIZE_TASKA 128 /*!< Define "taskA" task size */
//#define STACK_SIZE_TASKB 128 /*!< Define "taskA" task size */
#define STACK_SIZE_TASKC 128 /*!< Define "taskA" task size */
#define STACK_SIZE_TASK_UART 128 /*!< Define "taskA" task size */
#define STACK_SIZE_TASK_HDC1080 128 /*!< Define "taskA" task size */

/*---------------------------- Variable Define ------------------------------*/
//OS_STK taskA_stk[STACK_SIZE_TASKA];	/*!< Define "taskA" task stack */
//OS_STK taskB_stk[STACK_SIZE_TASKB];	/*!< Define "taskB" task stack */
OS_STK taskC_stk[STACK_SIZE_TASKC];	/*!< Define "led" task stack   */
OS_STK taskUart_stk[STACK_SIZE_TASK_UART];	/*!< Define "uart" task stack   */
OS_STK taskHDC1080_stk[STACK_SIZE_TASK_HDC1080];	/*!< Define "uart" task stack   */

OS_MutexID mutex_I2C;

/*---------------------------- Function Define ------------------------------*/
void rccConfig(void);

typedef struct {
	long m1;
	unsigned long m2;
	long m3;
}test_t;

const test_t par={345,20,130};

uint32_t ul_tmp;

/**
 *******************************************************************************
 * @brief       "taskA" task code
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 * @par Description
 * @details    This task use to crate mutex and flags,print message "taskA running".
 *             Indicate "taskA" had been executed.
 *******************************************************************************
 */
void taskA (unsigned int st) {
	unsigned int led_num;
	led_num=st;
	for (;;) {
		led_num++;
		CoTickDelay (50);
  }
}


/**
 *******************************************************************************
 * @brief       "taskB" task code
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 * @par Description
 * @details    This task use to print message "taskB running". Indicate "taskB"
 *             had been executed.
 *******************************************************************************
 */
void taskB (test_t* p) {
	unsigned int led_num;

	led_num=p->m3;
	for (;;) {
		led_num++;
		CoTickDelay (10);
		if (GPIO_ReadInputDataBit(USER_BUTTON_PORT, USER_BUTTON_PIN)) {

			uart1TxCmd(uart1Tx.buffer, 0x15);

			//ul_tmp = DMA1->ISR;
			CoTickDelay (50);
			while (GPIO_ReadInputDataBit(USER_BUTTON_PORT, USER_BUTTON_PIN));
		}
	}
}


/**
 *******************************************************************************
 * @brief       "led" task code
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 * @par Description
 * @details    This function use to blink led,and set flag for "taskA" and "taskB".
 *******************************************************************************
 */
#define BSRR_VAL 0x0300
void taskC (void* pdata) {
	//unsigned int led_num;
	while (1) {
		/* Set PC8 and PC9 */
		GPIOC->BSRR = BSRR_VAL;
		/* Delay some time */
		//for(times = 0; times < 500000; times++);
		CoTickDelay(50);
		/* Reset PC8 and PC9 */
		GPIOC->BRR = BSRR_VAL;
		/* Delay some time */
		//for(times = 0; times < 500000; times++);
		CoTickDelay(50);
	}
}


/**
 *******************************************************************************
 * @brief       main function
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 * @par Description
 * @details    none.
 *******************************************************************************
 */
int main(void) {
	uint16_t uiOsVersion;
	//SystemInit(); startup i�inde �a��r�l�yor.
	//SysTick_Config(SystemCoreClock / 100); CoInitOS function icinde tanimli.
	SystemCoreClockUpdate1();
	setParameters();
	rccConfig();
	initGpio();
	initUart1(sys_par.uart1_baudrate);
	initUartDma();
	uart1NvicConfig();
	initI2C();
	initTimers();

	uiOsVersion = CoGetOSVersion();
	mutex_I2C = CoCreateMutex();
	CoInitOS(); /*!< Initial CooCox CoOS */
	/*!< Create three tasks	*/
	CoCreateTask((FUNCPtr)taskC,(void *)0,2,&taskC_stk[STACK_SIZE_TASKC-1],STACK_SIZE_TASKC);
	CoCreateTask((FUNCPtr)taskUart1,(void *)0,2,&taskUart_stk[STACK_SIZE_TASK_UART-1],STACK_SIZE_TASK_UART);
	CoCreateTask((FUNCPtr)taskHDC1080,(void *)0,2,&taskHDC1080_stk[STACK_SIZE_TASK_HDC1080-1],STACK_SIZE_TASK_HDC1080);
	CoStartOS(); /*!< Start multitask*/

    while(1)
    {
    }
}
/**
 *******************************************************************************
 * @brief       Enable peripheral clocks
 *******************************************************************************
 */
void rccConfig(void) {
	/* GPIOA Peripheral clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* GPIOB Peripheral clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	/* GPIOC Peripheral clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	/* Enable USART1 APB clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Configure I2C Clock Source*/
	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI); //8 MHz clock source selected
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
}

/**
 *******************************************************************************
 * @brief       Check parameters of STM32 Std drivers
 *******************************************************************************
 */
void assert_failed(uint8_t* file, uint32_t line){
	while (1);
}
/* * * END OF FILE * * */

/* * * END OF FILE * * */