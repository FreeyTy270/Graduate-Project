/*
 * analysis.c
 *
 *  Created on: Apr 5, 2023
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
#include "string.h"
#include "math.h"

/* User-Created Includes */
#include "global.h"
#include "transform.h"
#include "analysis.h"

#define wErr 2 // Window error correction factor
#define NPBw 1.5 // Noise Power BW for Hann window
#define T FS/SAMPLES // Time Step

extern QueueHandle_t stats_mbx;
extern QueueHandle_t res_mbx;

int ansy_tx_fail = 0;
int ansy_rx_fail = 0;

unsigned long Total_mult = 0;
unsigned long Total_add = 0;

int ansys_pkgs = 0;

extern _Bool txmDone_flg;
_Bool ansyDone_flg = 0;


void ansys_Task(void * pvParameters)
{
	TickType_t lastWake = 0;
	TickType_t Period = pdMS_TO_TICKS(10);
	stats_t fft_res;

	float *temp_buf = NULL;

	while(1)
	{
		if(xQueueReceive(stats_mbx, &fft_res, Period)) // If analysis has been triggered
		{
			temp_buf = malloc((SMP_2+1)*sizeof(float)); // N/2+1 buffer for calculations

			ansys_pkgs++;
			for(int i = 0; i < SMP_2+1; i++) // Convert to single sided
			{
				if(i==0)
				{
					fft_res.res_buf[i] = mag(fft_res.res_buf[i])/SAMPLES; // Normalize magnitude of DC component
				}
				else
				{
					fft_res.res_buf[i] = sqrt(2)*mag(fft_res.res_buf[i])/SAMPLES; // Convert to Amplitude rms value
				}

				temp_buf[i] = creal(fft_res.res_buf[i])*wErr; // Correct windowed amplitude and transfer to smaller buffer
			}

			if(xQueueSend(res_mbx, temp_buf, 0)) ansys_pkgs++;// Send smaller buffer for printing

			else ansy_tx_fail++;

			free(temp_buf);

			/* For averages at end of the program */
			Total_mult += fft_res.mult_cnt;
			Total_add  += fft_res.add_cnt;

			if(txmDone_flg && uxQueueMessagesWaiting(stats_mbx) == 0)
			{
				ansyDone_flg = 1;
				vTaskSuspend(NULL);
			}
		}

		lastWake = xTaskGetTickCount();
		vTaskDelayUntil(&lastWake, Period);

	}
}





/**
  * @brief Magnitude calculation of complex number
  * @param N Complex number
  * @retval None complex floating point number
  */
float mag(float complex N)
{
	float r2 = creal(N)*creal(N);
	float i2 = cimag(N)*cimag(N);

	return sqrt(r2 + i2);
}
