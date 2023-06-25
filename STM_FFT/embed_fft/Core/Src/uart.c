/*
 * uart.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Ty Freeman
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "stm32l4xx_hal.h"

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "math.h"

#include "global.h"
#include "uart.h"
#include "adc.h"

#define STR_SZ 10

extern TaskHandle_t adc;
extern TaskHandle_t rdr;
extern TaskHandle_t tf;
extern TaskHandle_t ansys;
extern QueueHandle_t res_mbx;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern ADC_HandleTypeDef hadc1;

extern unsigned long int Total_mult;
extern unsigned long int Total_add;
extern unsigned int pkg_cnt;
extern float tm2full;

uint8_t rxbuf = '\0';

unsigned char caret[] = "\n\r> ";
unsigned char cr[] = "\n\r";
unsigned char bkspc[] = "\b\0";
unsigned char clr = '\0';

_Bool cr_flg = 0;
_Bool rd_flg = 0;
_Bool timLim_flg = 0;
_Bool bufRdy_flg = 1;
_Bool PROG_END = 0;
extern _Bool ansyDone_flg;

int Tx_fails = 0;
int Rx_fails = 0;
char nbuf[5];

int ttl_pkgs = 0;
int pkgs_sent = 0;
int uart_pkgs = 0;

/*Callback for UART receiver. Every 1 character triggers this callback function which does light processing of value*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	static int index = 0;

	HAL_UART_Transmit(&huart2, &rxbuf, 1, 2); //Echo character

	if(!cr_flg)
	{
		if((rxbuf == '\n' || rxbuf == '\r') && rd_flg) //If enter key has been hit
		{
			cr_flg = 1; // Set the flag which is checked in the reader task below
			rd_flg = 0; // Clear trigger flag
			index = 0;
			HAL_UART_Transmit(&huart2, cr, sizeof(cr), 2); // Echo character
		}
		else if((rxbuf == 'g' || rxbuf == 'G') && !rd_flg) // g or G for Go
		{
			rd_flg = 1; // Ready for read task
		}
		else if(rxbuf > 47 && rxbuf < 58 && index < 5)
		{
			nbuf[index] = rxbuf; // Collect up to 5 numbers in this buffer
			index++; // Increment buffer index
		}
		else if(rxbuf == 's' || rxbuf == 'S') // s or S for seconds
		{
			timLim_flg = 1; // User specified time limit
		}
		else if(rxbuf != ' ') // If not acceptable letter
		{
			HAL_UART_Transmit(&huart2, bkspc, sizeof(bkspc), 2); // Auto backspace
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) // UART Tx DMA transfer complete callback
{
	pkgs_sent++; // Keep track of packages sent
	if(ansyDone_flg && uxQueueMessagesWaitingFromISR(res_mbx) == 0) // Need adc done and all data sent
	{
		PROG_END = 1; // Signal program end
		HAL_UART_AbortTransmit(huart); // Turn off continuous printing
	}
	else
		bufRdy_flg = 1; // Signal buffer ready for new data
}

void read_Task(void * pvParameters)
{

	TickType_t lastWake = 0;
	TickType_t Period = pdMS_TO_TICKS(5);

	float fft_res[SMP_2+1];
	char TxBuf[(SMP_2+1)*STR_SZ];

	int avg_MC = 0;
	int avg_AC = 0;

	HAL_UART_Transmit(&huart2, caret, sizeof(caret), 2); // Print starting CMD caret

	while(1)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, &rxbuf, 1); // Begin DMA
		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);


		if(cr_flg)
		{
			cr_flg = 0; // Clear carriage return flag
			ttl_pkgs = atoi(nbuf); // Extract user set limit
			memset(nbuf,0,sizeof(nbuf)); // Clear number buffer
			if(timLim_flg) ttl_pkgs = (int) floor(ttl_pkgs/tm2full); // Calculate number of packages needed
			HAL_UART_Transmit(&huart2, cr, sizeof(cr), 2);
			xTaskCreate(adc_Task, "adc", 1024,(void *) &ttl_pkgs, PriorityNormal, &adc);
		}

		if(xQueueReceive(res_mbx, fft_res, 0) && bufRdy_flg) // Results ready and Tx buffer clear
		{

			uart_pkgs++;

			for(int i=0; i<SMP_2+1; i++)
			{
				/* Maintain standard 10 bytes of data after sprintf */
				if(fft_res[i] > 9 && fft_res[i] < 100)
					 sprintf(TxBuf+i*STR_SZ, "%.5f,", fft_res[i]);

				 else if(fft_res[i] > 99)
					 sprintf(TxBuf+i*STR_SZ, "%.4f,", fft_res[i]);

				 else
					 sprintf(TxBuf+i*STR_SZ, "%.6f,", fft_res[i]);

			}

				HAL_UART_Transmit_DMA(&huart2, (unsigned *) TxBuf, sizeof(TxBuf)); // Start DMA
				bufRdy_flg = 0; // Tx buffer being used by new data

		}


		if(PROG_END) // All data printed and ADC done
		{
			PROG_END = 0; // Clr flag
			avg_MC = ceil((float) Total_mult/pkg_cnt); // Calculate average number of multiplications per transform
			avg_AC = ceil((float) Total_add/pkg_cnt); // Calculate average number of additions per transform

			Tx_fails = trans_tx_fail + ansy_tx_fail + adc_tx_fail;
			Rx_fails = trans_rx_fail + ansy_rx_fail;

			/* Print Stats Message Block */
			printf("\n\n\r***** STATS *****\n\n\r");
			printf("Average Mult Ops per transform:\t%d\n\r", avg_MC);
			printf("Average Add Ops transform:\t%d\n\n\r", avg_AC);

			printf("Number of Rx Failures: %d\n\r", Rx_fails);
			printf("Number of Tx Failures: %d\n\n\r", Tx_fails);
			printf("Packages seen by Task:\n\r");
			printf("\t\tADC\t%d\n\r", pkg_cnt);
			printf("\t\tTXM\t%d\n\r", txm_pkgs);
			printf("\t\tANSY\t%d\n\r", ansys_pkgs);
			printf("\t\tUART\t%d\n\r", uart_pkgs);
			printf("\t\tSENT\t%d\n\r", pkgs_sent);
		}


		lastWake = xTaskGetTickCount();
		vTaskDelayUntil(&lastWake, Period);
	}
}
