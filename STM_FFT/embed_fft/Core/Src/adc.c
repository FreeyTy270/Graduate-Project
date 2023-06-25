/*
 * adc.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Ty Freeman
 */

/* System Includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "stm32l4xx_hal.h"

/* Lib Includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

/* User-Created Includes */
#include "global.h"
#include "adc.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_adc1;
extern QueueHandle_t buffer_mbx;
extern int num_emptyBuf;
extern unsigned int quiet_cnt;

uint16_t RWM[SAMPLES]; // Capture buffer

/* Control Flags */
_Bool full = 0;
_Bool pkgRdy_flg = 0;
_Bool lim_flg = 0;
_Bool adcDone_flg = 0;

/* Counters */
unsigned int pkg_cnt = 0;
int idx = 0;
int adc_tx_fail = 0;


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) // ADC callback function
{
    RWM[idx] = HAL_ADC_GetValue(&hadc1); // Acquire ADC value
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle LED
    if(idx == SAMPLES-1) // Check index value. Restart?
    {
		full = 1;
    	idx = SMP_2; // Only saving half of samples for window overlapping
    }
    else
    	idx++;
}

void adc_Task(void * pvParameters)
{
	TickType_t lastwake = 0;

	HAL_ADC_Start_IT(&hadc1); // Start ADC
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Start ADC trigger timer

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1); // Turn on LED to signal ADC start


	uint16_t *package = NULL;
	pkg_cnt = 0; // Reset for repeat trials
	quiet_cnt = num_emptyBuf; // Reset quiet cnt number at start of ADC task

	/* User Set Limit Signals */
	_Bool lim_set = 0;
	int *pkg_lim = (int *) pvParameters;
	if(*pkg_lim > 0) // Pull pkg limit from parameter
	{
		lim_set = 1;
	}

	while(1)
	{
		if(full)
		{
			package = (uint16_t *)malloc(SAMPLES*sizeof(uint16_t)); // Create package buffer
			memcpy(package, RWM, SAMPLES*sizeof(uint16_t)); // Move values from capture to package buffer
			memmove(RWM, RWM+SMP_2, SMP_2*sizeof(uint16_t)); // Shift capture buffer values down for window overlap


			if(xQueueSend(buffer_mbx, package, 1)) // Send package to transform task for processing
			{
				pkg_cnt++; // Count number of packages processed
				pkgRdy_flg = 1; // Data package in mailbox ready for transform
			}

			else adc_tx_fail += 1; // If failed to post, keep track of failure

			if(lim_set && pkg_cnt == *pkg_lim) lim_flg = 1; // Signal time to stop

			full = 0; // Reset full flg

			free(package); // Release package space

			if(lim_flg || sigDone_flg) // Two possible ending conditions
			{
				/* Turn off Timer and ADC before deleting the adc task */
				HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
				HAL_ADC_Stop_IT(&hadc1);

				adcDone_flg = 1; // Signal program that adc is done
				if(sigDone_flg) pkg_cnt -= num_emptyBuf; // Remove empty buffers from total count

				sigDone_flg = 0; // Clear signal flag
				lim_flg = 0; // Clear limit flag

				vTaskDelete(NULL);
			}
		}


		lastwake = xTaskGetTickCount();
		vTaskDelayUntil(&lastwake, pdMS_TO_TICKS(30)); // Task suspension for 30 ms
	}
}
