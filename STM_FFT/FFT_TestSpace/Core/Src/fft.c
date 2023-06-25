/*
 * fft.c
 *
 *  Created on: Mar 18, 2023
 *      Author: Ty Freeman
 */

/* System Includes */
#include "stm32l4xx_hal.h"

/* lib Includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

/* User-Created Includes */
#include "fft.h"

extern TIM_HandleTypeDef htim6;
extern uint8_t tim_turnover;

float complex twexp = -2*PI*I/SAMPLES;

void classic_FFT(stats_t *results)
{
	int a = SAMPLES/2;
	int p = 0;

	results->mult_cnt = 0;
	results->add_cnt = 0;
	results->time = 0;

	tim_turnover = 0;
	TIM6->CNT = 0;
	HAL_TIM_Base_Start(&htim6);
	int stages = log_2(SAMPLES);

	for(int j = 1; j <= stages; j++)
	{
		for(int k = 0; k < SAMPLES; k++)
		{
			if(!(k & a))
			{
				p = bit_reverse(stages, k >> (stages - j));
				float complex x = results->res_buf[k];
				float complex x1 = cexp(twexp*p)*results->res_buf[k+a];
				results->res_buf[k] = x + x1;
				results->res_buf[k + a] = x - x1;
				results->mult_cnt += 1;
				results->add_cnt += 2;
			}
		}

		a >>= 1;
	}

	for(int i = 0; i < SAMPLES; i++)
	{
		int p = bit_reverse(stages, i);

		if(i < p)
		{
			float complex n = results->res_buf[i];
			results->res_buf[i] = results->res_buf[p];
			results->res_buf[p] = n;
		}
	}

	results->time = TIM6->CNT;
	HAL_TIM_Base_Stop(&htim6);

	if(tim_turnover > 0)
	{
		results->time += 65535*tim_turnover;
	}
}

int bit_reverse(int sz, int index)
{
	int p = 0;

	for(int i = 0; i <= sz; i++)
	{
		if(index & (1 << (sz - i)))
		{
			p |= 1 << (i - 1);
		}
	}

	return p;
}

int log_2(unsigned int N)
{
	int pow = 0;

	while(N)
	{
		N >>= 1;
		pow++;
	}

	return pow - 1;
}

float mag(float complex N)
{
	float r2 = creal(N)*creal(N);
	float i2 = cimag(N)*cimag(N);

	return sqrt(r2 + i2);
}
