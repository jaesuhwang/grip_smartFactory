/*
 * tdma_n.h
 *
 *  Created on: 2022. 2. 8.
 *      Author: lucky
 */

#ifndef INC_TDMA_N_H_
#define INC_TDMA_N_H_

#include "main.h"

/**
 * This node MAC address
 */
//extern uint8_t node_addr;


/**
 * The size of the mac_data_payload buffer.
 */
#define TDMA_PAYLOAD_SIZE 60

void mac_init(uint8_t channel);
uint16_t beacon_received(void);
uint16_t sync_detected(void);


extern uint8_t* const mac_payload;
void mac_set_access_allowed_cb(uint16_t (*cb)(void));

extern uint8_t node_addr;


#endif /* INC_TDMA_N_H_ */
