/*
 * globals.h
 *
 *  Created on: Dec 1, 2022
 *      Author: Ty Freeman
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include <inttypes.h>

#define SR 20000
#define todig 4096/3.3
#define toreal 3.3/4096

typedef struct stats {
	int time;
	int operations;
}stats_t;

/* RTOS Priority Enum */
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

/* Status Flags */
extern _Bool full;
extern _Bool cr_flg;
extern _Bool cmd_flg;
extern _Bool rd_flg;
extern _Bool out_flg;
extern _Bool adc_done;

extern double sum;

#endif /* INC_GLOBALS_H_ */
