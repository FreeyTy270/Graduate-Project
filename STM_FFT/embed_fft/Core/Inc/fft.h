/*
 * fft.h
 *
 *  Created on: Nov 7, 2022
 *      Author: Ty Freeman
 */

#ifndef INC_FFT_H_
#define INC_FFT_H_

#include "global.h"

void FFT(stats_t *results);
int log_2(unsigned int N);
int bit_reverse(int sz, int index);

#endif /* INC_FFT_H_ */
