
#ifndef CK_TIME_H_
#define CK_TIME_H_

#include "fsl_common.h"

void CK_TIME_Init(void);

uint32_t CK_TIME_GetMicroSec(void);

uint32_t CK_TIME_GetMilliSec(void);

void CK_TIME_DelayMicroSec(uint32_t usec);

void CK_TIME_DelayMilliSec(uint32_t msec);

#endif /* CK_TIME_H_ */
