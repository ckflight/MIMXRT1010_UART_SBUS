
#ifndef CK_CIRCULARBUFFER_H_
#define CK_CIRCULARBUFFER_H_

#include "MIMXRT1011.h"
#include "stdbool.h"

typedef struct {

	uint8_t * buffer;
    int head;
    int tail;
    int size;

}circularBuffer_t;

void CK_CIRCULARBUFFER_Init(circularBuffer_t* cb, int size);

int CK_CIRCULARBUFFER_BufferWrite(circularBuffer_t* c, uint8_t data);

int CK_CIRCULARBUFFER_BufferWriteMulti(circularBuffer_t* c, uint8_t* buff, uint32_t length);

int CK_CIRCULARBUFFER_BufferRead(circularBuffer_t* c, uint8_t* data);

void CK_CIRCULARBUFFER_GetBuffer(circularBuffer_t* c, uint8_t* buff, uint32_t* numOfElement);

int CK_CIRCULARBUFFER_GetAvailable(circularBuffer_t* c);

bool CK_CIRCULARBUFFER_IsBufferEmpty(circularBuffer_t* c);

bool CK_CIRCULARBUFFER_IsBufferFull(circularBuffer_t* c);

#endif /* CK_CIRCULARBUFFER_H_ */
