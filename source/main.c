
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1011.h"
#include "fsl_debug_console.h"

#include "fsl_iomuxc.h"
#include "fsl_lpuart.h"

#include "CK_TIME.h"
#include "CK_CIRCULARBUFFER.h"

#define LPUART_CLK_FREQ BOARD_DebugConsoleSrcFreq()

#define BUFFER_SIZE		128

#define SBUS_STARTBYTE         		0x0F
#define SBUS_ENDBYTE           		0x00
#define SBUS_PACKET_SIZE            25 // 1 Start Byte + 23 Byte Payload + 1 End byte
#define SBUS_FAILSAFE_INACTIVE 		0
#define SBUS_FAILSAFE_ACTIVE   		1
#define SBUS_FAILSAFE_CHANNEL		19 // 0 to 18 makes 19 channels


uint8_t sbus_buffer[SBUS_PACKET_SIZE];
uint8_t buffer_index = 0;

void CK_SBUS_Process(void);
void CK_SBUS_Decode(void);

circularBuffer_t sbus_cb;

int _channels[19];

int main(void){

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    CK_TIME_Init();

    CK_CIRCULARBUFFER_Init(&sbus_cb, BUFFER_SIZE);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_01_LPUART4_RXD, 0U);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_02_LPUART4_TXD, 0U);

    // Slew Rate Field: Slow Slew Rate
	// Drive Strength Field: R0/4
	// Speed Field: fast(150MHz)
	// Open Drain Enable Field: Open Drain Disabled
	// Pull / Keep Enable Field: Pull/Keeper Enabled
	// Pull / Keep Select Field: Keeper
	// Pull Up / Down Config. Field: 100K Ohm Pull Down
	// Hyst. Enable Field: Hysteresis Disabled
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_01_LPUART4_RXD, 0x10A0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_02_LPUART4_TXD, 0x10A0U);

    /*
     * config.baudRate_Bps 		= 115200U;
     * config.parityMode 		= kLPUART_ParityDisabled;
     * config.stopBitCount 		= kLPUART_OneStopBit;
     * config.txFifoWatermark 	= 0;
     * config.rxFifoWatermark 	= 0;
     * config.enableTx 			= false;
     * config.enableRx 			= false;
     */
    lpuart_config_t config;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps 	= 100000;
    config.parityMode 		= kLPUART_ParityEven;
    config.stopBitCount		= kLPUART_TwoStopBit;
    config.dataBitsCount 	= kLPUART_EightDataBits;
    config.enableTx     	= true;
    config.enableRx     	= true;


    LPUART_Init(LPUART4, &config, LPUART_CLK_FREQ);

    LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);
    EnableIRQ(LPUART4_IRQn);

    while(1){

    	uint32_t time1 = CK_TIME_GetMicroSec();
    	CK_SBUS_Process();
    	uint32_t time2 = CK_TIME_GetMicroSec() - time1;

        PRINTF("%d\n",time2);

    	CK_TIME_DelayMilliSec(10);

    }
}

void CK_SBUS_Process(void){

    while(!CK_CIRCULARBUFFER_IsBufferEmpty(&sbus_cb)){

    	uint8_t rx;

        CK_CIRCULARBUFFER_BufferRead(&sbus_cb, &rx);

        if (buffer_index == 0 && rx != SBUS_STARTBYTE) {
            continue; // incorrect start byte, out of sync go to while with next iteration
        }

        sbus_buffer[buffer_index++] = rx;

        if (buffer_index == SBUS_PACKET_SIZE){
            buffer_index = 0;
            if (sbus_buffer[24] != SBUS_ENDBYTE) {
                continue; // incorrect start byte, out of sync go to while with next iteration
            }
            CK_SBUS_Decode();
        }
    }
}

void CK_SBUS_Decode(void){

    _channels[0]  = ((sbus_buffer[1]      | sbus_buffer[2]  << 8)                        & 0x07FF);
    _channels[1]  = ((sbus_buffer[2]  >>3 | sbus_buffer[3]  << 5)                        & 0x07FF);
    _channels[2]  = ((sbus_buffer[3]  >>6 | sbus_buffer[4]  << 2  | sbus_buffer[5]<<10)  & 0x07FF);
    _channels[3]  = ((sbus_buffer[5]  >>1 | sbus_buffer[6]  << 7)                        & 0x07FF);
    _channels[4]  = ((sbus_buffer[6]  >>4 | sbus_buffer[7]  << 4)                        & 0x07FF);
    _channels[5]  = ((sbus_buffer[7]  >>7 | sbus_buffer[8]  << 1  | sbus_buffer[9]<<9)   & 0x07FF);
    _channels[6]  = ((sbus_buffer[9]  >>2 | sbus_buffer[10] << 6)                        & 0x07FF);
    _channels[7]  = ((sbus_buffer[10] >>5 | sbus_buffer[11] << 3)                        & 0x07FF);
    _channels[8]  = ((sbus_buffer[12]     | sbus_buffer[13] << 8)                        & 0x07FF);
    _channels[9]  = ((sbus_buffer[13] >>3 | sbus_buffer[14] << 5)                        & 0x07FF);
    _channels[10] = ((sbus_buffer[14] >>6 | sbus_buffer[15] << 2  | sbus_buffer[16]<<10) & 0x07FF);
    _channels[11] = ((sbus_buffer[16] >>1 | sbus_buffer[17] << 7)                        & 0x07FF);
    _channels[12] = ((sbus_buffer[17] >>4 | sbus_buffer[18] << 4)                        & 0x07FF);
    _channels[13] = ((sbus_buffer[18] >>7 | sbus_buffer[19] << 1  | sbus_buffer[20]<<9)  & 0x07FF);
    _channels[14] = ((sbus_buffer[20] >>2 | sbus_buffer[21] << 6)                        & 0x07FF);
    _channels[15] = ((sbus_buffer[21] >>5 | sbus_buffer[22] << 3)                        & 0x07FF);

    if(sbus_buffer[23] & 0x0001) _channels[16] = 2047;
    else _channels[16] = 0;

    if((sbus_buffer[23] >> 1) & 0x0001) _channels[17] = 2047;
    else _channels[17] = 0;

    //Channel 19 is FAILSAFE FLAG 18 means Channel 19
    if ((sbus_buffer[23] >> 3) & 0x0001) _channels[18] = SBUS_FAILSAFE_ACTIVE;
    else _channels[18] = SBUS_FAILSAFE_INACTIVE;

}

void LPUART4_IRQHandler(void){

    uint8_t rxData;

    // If new data arrived
    if((kLPUART_RxDataRegFullFlag) & LPUART_GetStatusFlags(LPUART4)){

    	rxData = LPUART_ReadByte(LPUART4);

        // If ring buffer is not full, add data to ring buffer
    	if(!CK_CIRCULARBUFFER_IsBufferFull(&sbus_cb)){
			CK_CIRCULARBUFFER_BufferWrite(&sbus_cb, rxData);
		}

    }

}




