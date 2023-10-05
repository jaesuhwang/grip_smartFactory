/*
 * tdma_n.c
 *
 *  Created on: 2022. 2. 8.
 *      Author: lucky
 */

#include <stdlib.h>
#include <string.h>

#include "tdma_n.h"
#include "tdma_frames.h"
#include "tdma_timings.h"
#include "timerB.h"

#include "cc1120.h"
#include "cc112x_spi.h"
#include "usart.h"

#include "sleep.h"

uint8_t node_addr = 9;
//uint8_t node_addr=2;
//uint8_t node_addr=3;
//uint8_t node_addr=4;
//uint8_t node_addr=5;
//uint8_t node_addr=6;
//uint8_t node_addr=7;
//uint8_t node_addr=8;

// 940 ~ 946.3

#define LED_RED_OFF()	\
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET)
#define LED_RED_ON()	\
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_SET)

#define LED_BLUE_OFF()	\
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET)
#define LED_BLUE_ON()	\
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET)

#define LED_WHITE_OFF()	\
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET)
#define LED_WHITE_ON()	\
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET)

#define PACKET_LEN_IDX			0
#define PACKET_DEST_IDX			1
#define PACKET_SRC_IDX			2

enum {
	STATE_BEACON_SEARCH, // looking for a beacon
	STATE_ATTACHING_WAIT_TX, // beacon found, waiting random time to send attach
	STATE_ATTACHING_WAIT_RX, // attach sent, waiting for response
	STATE_ATTACHED
};

static uint16_t sync_time;
static uint8_t coord_addr;
static uint8_t my_slot;
static uint8_t state;

#define ALARM_BEACON    TIMERB_ALARM_CCR0
#define ALARM_TIMEOUT 	TIMERB_ALARM_CCR1
#define ALARM_SEND      TIMERB_ALARM_CCR2

volatile uint8_t send_ready = 0;

/* STATIC PROTOTYPES */
static void set_rx(void);
static uint16_t beacon_rx(void);
static uint16_t control_send(void);
static uint16_t control_sent(void);
static uint16_t beacon_timeout(void);
static uint16_t slot_send(void);
static uint16_t slot_sent(void);

/* STATIC VARIABLES */
static beacon_msg_t beacon_msg;
static uint16_t beacon_sync_time;
static uint16_t beacon_eop_time;
static uint16_t beacon_timeout_count;

static control_msg_t control_msg;
static uint16_t attach_backoff;

uint16_t (*access_allowed_cb)(void);

static data_msg_t data_msg;
uint8_t *const mac_payload = data_msg.payload;

uint8_t beacon2sleeptime[2];

void beaconGetSleepTimeData(uint8_t *beaconData, uint8_t *buf) {
	uint8_t *data = (uint8_t*) &beaconData[4];

	buf[0] = data[(node_addr - 1) * 2 + 0];
	buf[1] = data[(node_addr - 1) * 2 + 1];
}

void mac_init(uint8_t chaanel) {
	uint8_t writeByte;

//	LED_WHITE_OFF();
// 	initialize the timerB
	timerB_init();
//	timerB_start_ACLK_div(TIMERB_DIV_1);

#if PC_TEST_MODE
	cc1120_init(chaanel);
	#else
	cc1120_init();
#endif

	cc1120_cmd_idle();

	//cc1120_cfg_white_data(CC1120_DATA_WHITENING_ENABLE);
	//	CC1120_SpiCmdStrobe(CC112X_SIDLE);
	//		//Wait for radio to enter IDLE state before issuing the SPWD command
	//	do
	//	{
	//		rc = CC1120_SpiCmdStrobe(CC112X_SNOP);
	//	//__no_operation();
	//	} while ((rc & 0xF0) != 0x00);
	//	CC1120_SpiCmdStrobe(CC112X_SRX);

	//	writeByte = 0x40;
	//	CC1120_SpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
	//	writeByte = 0x78; CC1120_SpiWriteReg(CC112X_FIFO_CFG, &writeByte, 1);
	//	writeByte = 0x00; CC1120_SpiWriteReg(CC112X_IOCFG0, &writeByte, 1);
	//	writeByte = 0x06; CC1120_SpiWriteReg(CC112X_IOCFG2, &writeByte, 1);
	//	writeByte = 0x06; CC1120_SpiWriteReg(CC112X_IOCFG3, &writeByte, 1);

	// go to IDLE after RX and TX
#if 1
	//cc1120_cfg_rxoff_mode(CC1120_RXOFF_MODE_STAY_RX);
	cc1120_cfg_rxoff_mode(CC1120_RXOFF_MODE_IDLE);
	cc1120_cfg_txoff_mode(CC1120_TXOFF_MODE_IDLE);

	cc1120_gdo0_register_callback(beacon_received);		// payload는 rising 으로 캡쳐
	cc1120_gdo2_register_callback(sync_detected);		// sync 는 falling 으로 캡쳐
#endif

	timerB_init();

#if 1
//	state = STATE_BEACON_SEARCH;
	state = STATE_ATTACHED;
//	printf("BEACON SEARCH\n");
	my_slot = -1;
	set_rx();

	// initialize the flag
	send_ready = 0;
	// reset the callback
	access_allowed_cb = 0x0;

	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI9_5_IRQn);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI15_10_IRQn);
//	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 1);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

#endif
}

uint16_t packetCounter = 0;

int16_t mac_is_access_allowed(void) {
	return (send_ready == 0);
}

void mac_send(void) {
	send_ready = 1;
}

void mac_set_access_allowed_cb(uint16_t (*cb)(void)) {
	access_allowed_cb = cb;
}

static void set_rx(void) {
	// idle, flush, calibrate
	cc1120_cmd_idle();
	cc1120_cmd_flush_rx();
	cc1120_cmd_flush_tx();

	cc1120_cmd_calibrate();	

	// set RX
	cc1120_cmd_rx();
	LED_RED_OFF();
}

volatile uint32_t attachsendTime;
uint8_t old_statedd = 0xff;
uint8_t packetReceived;
uint8_t attached_rr;

uint8_t sentComplete = 1;
uint16_t beacon_received(void) {
	uint8_t marcStatus;
	uint8_t temp_byte;
	uint8_t rxBytes, i;
	uint8_t rxBuffer[512] = { 0, };
	uint16_t now;
	uint8_t coord, seq;

	now = timerB_time();
	LED_RED_ON();
#if 1
	CC1120_SpiReadReg(CC112X_MARC_STATUS1, &temp_byte, 1);
	if ((temp_byte & 0x09) || (temp_byte & 0x0a)) {
		//CC1120_SpiCmdStrobe(CC112X_SFRX);
		goto err;
		//return 0;
	}

	else if (temp_byte & 0x80) {
		CC1120_SpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);

		// Check that we have bytes in fifo
		if (rxBytes != 0) {
			// Read marcstate to check for RX FIFO error
			CC1120_SpiReadReg(CC112X_MARCSTATE, &marcStatus, 1);

			// Mask out marcstate bits and check if we have a RX FIFO error
			if ((marcStatus & 0x1F) == 0x11/*RX_FIFO_ERROR*/) {
				// Flush RX Fifo
				CC1120_SpiCmdStrobe(CC112X_SFRX);
				goto err;
			} else {
				// Read n bytes from rx fifo
				CC1120_SpiReadRxFifo(rxBuffer, rxBytes);

				//for(i = 0; i < rxBytes; i ++)
				//	printf("rxBuffer[%d]: %d\n", i, rxBuffer[i]);

				// Check CRC ok (CRC_OK: bit7 in second status byte)
				// This assumes status bytes are appended in RX_FIFO
				// (PKT_CFG1.APPEND_STATUS = 1.)
				// If CRC is disabled the CRC_OK field will read 1
				if (rxBuffer[rxBytes - 1] & 0x80) {
//					if ( rxBytes != )
					memcpy((uint8_t*) &beacon_msg, &rxBuffer, BEACON_LENGTH);

//					for ( int i = 0 ; i < rxBytes; i ++)
//					{
//						printf("idx=[%d]  rxByte=%02x\r\n", i , rxBuffer[i]);
//					}
					//data
					// check length, type
					if ((beacon_msg.hdr.length != (BEACON_LENGTH - 1))
							|| (HEADER_GET_TYPE(beacon_msg.hdr) != BEACON_TYPE)) {
						//set_rx();
						goto err;
					}
					// unset timeout alarm
					timerB_unset_alarm(ALARM_TIMEOUT);

					coord = HEADER_GET_ADDR(beacon_msg.hdr);
					seq = beacon_msg.seq;
					beacon_eop_time = now;

//					if (state==STATE_BEACON_SEARCH) {
//						// we were looking for a beacon, store the coordinator addr
//						coord_addr = coord;
//					} else if (coord != coord_addr) {
//						// beacon from unknown coordinator
//						//set_rx();
//						goto err;
//					}

					beaconGetSleepTimeData(&beacon_msg, &beacon2sleeptime); // 분초 파싱부분
#if 0
					printf("\r\ndata :");
					for (int i = 0 ; i <BEACON_PACKET_SIZE_MAX; i++ )
					{
						printf("%02x ",beacon_msg.data[i]);
					}
#endif

					LED_RED_OFF();

					// reset timeout count
					beacon_timeout_count = 0;

					// save beacon time
					beacon_sync_time = sync_time - BEACON_OVERHEAD;
					//printf("%d\r\n",beacon_sync_time);
					// set alarm to receive beacon
					timerB_set_alarm_from_time(ALARM_BEACON,  // alarm #
							BEACON_PERIOD,  // ticks
							0,  // no period
							beacon_sync_time - SAFETY_TIME); // reference
					timerB_register_cb(ALARM_BEACON, beacon_rx);
#if 1
					switch (state) {
					case STATE_BEACON_SEARCH:
						// pick a random backoff between 1 and 16
						attach_backoff = 1 + (rand() & 0xF);
						//attach_backoff = 4;

						// update state
						state = STATE_ATTACHING_WAIT_TX;
						break;

					case STATE_ATTACHING_WAIT_TX:
						// decrement count
						attach_backoff--;
						//printf("bf=%d\r\n", attach_backoff);
						if (attach_backoff == 0) {
							// prepare attach request frame
							control_msg.hdr.length = CONTROL_LENGTH - 1;
							HEADER_SET_ADDR(control_msg.hdr, node_addr);
							HEADER_SET_TYPE(control_msg.hdr, CONTROL_TYPE);
							CONTROL_SET_TYPE(control_msg, CONTROL_ATTACH_REQ);
							CONTROL_SET_ADDR(control_msg, coord_addr);
							// set timer to send attach request
							timerB_set_alarm_from_time(ALARM_SEND,
							CTRL_SLOT * SLOT_LENGTH_CTRL, // ticks
									0, beacon_sync_time);

							timerB_register_cb(ALARM_SEND, control_send);
							LED_WHITE_ON();
							// update state
							state = STATE_ATTACHING_WAIT_RX;
							attach_backoff = 0;
						}
						break;

					case STATE_ATTACHING_WAIT_RX: {
						uint8_t becon_msg_t, addr;
						//printf("type=%02x addr= %02x\r\n", becon_msg_t, addr);
//				        	for ( int i = 0 ; i < rxBytes; i++)
//				        	{
//				        		printf("rx[%d] =%02x",i, rxBuffer[i]);
//				        	}
//				        	printf("\r\n");

						if ((CONTROL_GET_TYPE(beacon_msg) == CONTROL_ATTACH_OK)
								&& (CONTROL_GET_ADDR(beacon_msg) == node_addr)) {
							// store my_slot
							my_slot = beacon_msg.data;
							//printf("slot =%d\r\n", my_slot);
							state = STATE_ATTACHED;
						} else {
							// attach failed, retry at next beacon
							//printf("no attach %c\r\n", beacon_msg.ctl &0xf0);
							state = STATE_BEACON_SEARCH;
						}
					}
						break;
					case STATE_ATTACHED:
						if (1 /*send_ready*/) {
							// prepare data frame

							{

								data_msg.hdr.length = DATA_LENGTH - 1;
								HEADER_SET_TYPE(data_msg.hdr, DATA_TYPE);
								HEADER_SET_ADDR(data_msg.hdr, node_addr);

								timerB_set_alarm_from_time(ALARM_SEND, // alarm #
										node_addr * SLOT_LENGTH, // ticks
										0, // period
										beacon_sync_time); // ref
								// set alarm callback
								timerB_register_cb(ALARM_SEND, slot_send);

								//								 put the data in the FIFO
								//								cc1101_fifo_put((uint8_t*)&data_msg, data_msg.hdr.length+1);
								CC1120_SpiWriteTxFifo((uint8_t*) &data_msg,
										data_msg.hdr.length + 1);

								//PacketSend(0x00, MyNodeID, 0x01);
								send_ready = 0;
								if (access_allowed_cb && access_allowed_cb()) {
									// if wanted we return 1 to wake the CPU up
									//state = 0;
									return 1;
								}
							}
						}
						break;

					}
#endif
				} else
					goto err;

			}
		}
	}
#endif
	err: set_rx();
	return 0;
}

uint16_t sync_detected(void) {
	sync_time = timerB_time();
	return 0;
}

static uint16_t beacon_rx(void) {
	// wake up radio
	set_rx();

	// set callback
	cc1120_gdo0_register_callback(beacon_received);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI9_5_IRQn);
	//cc1101_gdo0_int_enable();

	// set alarm for beacon timeout
	timerB_set_alarm_from_now(ALARM_TIMEOUT,  // alarm #
			TIMEOUT_TIME,  // ticks
			0);  // no period
	timerB_register_cb(ALARM_TIMEOUT, beacon_timeout);
	return 0;
}

static uint16_t beacon_timeout(void) {
	// put radio to sleep
	//cc1101_cmd_pwd();
	LED_RED_OFF();

	// increase timeout count
	beacon_timeout_count++;

	// if too many timeouts, reset state
	if (beacon_timeout_count >= 500/*TIMEOUT_COUNT_MAX*/) {
		set_rx();
		//state = STATE_BEACON_SEARCH;
		state = STATE_ATTACHED;
		//printf("BEACON_SEARCH\n");
		return 0;
	}

	// reset alarm to receive beacon
	timerB_set_alarm_from_time(ALARM_BEACON,  // alarm #
			BEACON_PERIOD * (beacon_timeout_count + 1),  // ticks
			0,  // period (same)
			beacon_sync_time - (SAFETY_TIME * (beacon_timeout_count + 1))); // reference

	return 0;
}

static uint16_t control_send(void) {
	LED_BLUE_ON();
	cc1120_gdo0_register_callback(control_sent);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI9_5_IRQn);

	cc1120_cmd_idle();
	cc1120_cmd_flush_tx();
	cc1120_cmd_flush_rx();

	// 1.2khz일시
	// Strobe TX to send packet
	cc1120_cmd_tx();
	CC1120_SpiWriteTxFifo((uint8_t*) &control_msg, control_msg.hdr.length + 1);
	//printf("control send\r\n");
	// 1.2khz 이상일시
	//send_packet((uint8_t*)&control_msg, control_msg.hdr.length+1);

	LED_BLUE_OFF();
	return 0;
}

static uint16_t control_sent(void) {
	LED_BLUE_OFF();
	return 0;
}

static uint16_t slot_send(void) {
	uint8_t statusByte;
	uint8_t marcstate;
	printf("sending\r\n");
	//LED_BLUE_ON();

	cc1120_cmd_tx(); // 실제 보내는곳
	cc1120_gdo0_register_callback(slot_sent);


//	CC1120_SpiReadReg(CC112X_MARC_STATUS1, &statusByte, 1);
//	if (statusByte == 0x07 ||  statusByte == 0x08)
//	{
//		//Flush FIFO
//		cc1120_cmd_flush_tx();
//	}
//	else
//	{
//		//cc1120_gdo0_register_callback(beacon_sent);
//			//__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI0_IRQn);
//		send_packet((uint8_t*)&data_msg, data_msg.hdr.length+1);
//	}
	return 0;
}

static uint16_t slot_sent(void) {
	// put radio to sleep
	//cc1120_cmd_pwd();
//    LED_GREEN_OFF();
	LED_BLUE_OFF();
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
#if 1
	//sleep_deinit();
	//SleepModeEntryFunction(beacon2sleeptime[0],beacon2sleeptime[1]);
	SleepModeEntryFunction(0,5);
#endif
	return 0;
}

