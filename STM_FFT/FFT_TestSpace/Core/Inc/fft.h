/*
 * fft.h
 *
 *  Created on: Mar 18, 2023
 *      Author: Ty Freeman
 */

#ifndef INC_FFT_H_
#define INC_FFT_H_

#include "complex.h"

#define PI 3.14159265358979323846
#define FS 44100
#define SAMPLES 4096
#define SMP_2 SAMPLES/2

typedef struct stats{
	unsigned int mult_cnt;
	unsigned int add_cnt;
	unsigned int time;
	float complex res_buf[SAMPLES];
}stats_t;

float mag(float complex N);
int log_2(unsigned int N);
int bit_reverse(int sz, int index);
void classic_FFT(stats_t *results);
#endif /* INC_FFT_H_ */
