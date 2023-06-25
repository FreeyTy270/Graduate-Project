/*
 * global.h
 *
 *  Created on: Jan 31, 2023
 *      Author: Ty Freeman
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "complex.h"

#define PI 3.14159265358979323846
#define SYS_FREQ 80000000
#define SAMPLES 256
#define FS 5000
#define dt 1/FS
#define SMPx2 SAMPLES*2
#define SMP_2 SAMPLES/2

extern unsigned int pkg_cnt;
extern int txm_pkgs;
extern int ansys_pkgs;

/* Control Flags */
extern _Bool ansy_flg;
extern _Bool pkgRdy_flg;
extern _Bool adcDone_flg;
extern _Bool sigDone_flg;
extern _Bool bufEmpty_flg;
extern _Bool resRdy_flg;

/* Fail Counters */
extern int adc_tx_fail;
extern int trans_tx_fail;
extern int trans_rx_fail;
extern int ansy_tx_fail;
extern int ansy_rx_fail;

extern double step;

enum  Priority{
  PriorityIdle,          ///< priority: idle (lowest)
  PriorityLow,                    ///< priority: low
  PriorityBelowNormal,          ///< priority: below normal
  PriorityNormal,          ///< priority: normal (default)
  PriorityAboveNormal,          ///< priority: above normal
  PriorityHigh,          ///< priority: high
  PriorityRealtime,          ///< priority: realtime (highest)
  PriorityError =  0x84        ///< system cannot determine priority or thread has illegal priority
};

typedef struct stats{
	unsigned int pkg_num;
	unsigned long mult_cnt;
	unsigned long add_cnt;
	int zCnt;
	float complex res_buf[SAMPLES];
}stats_t;


#endif /* INC_GLOBAL_H_ */
