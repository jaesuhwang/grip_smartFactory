/*
 * tdma_common.h
 *
 *  Created on: 2022. 2. 7.
 *      Author: lucky
 */

#ifndef INC_TDMA_COMMON_H_
#define INC_TDMA_COMMON_H_

#include "main.h"
#include "tdma_userconfig.h"

#define FRAME_HEADER_LENGTH 5
//#define MAX_PACKET_LENGTH 119

#define MAX_COORD_SEND_LENGTH 15
#define MAX_BEACON_DATA_LENGTH 55


/* Type def */
enum phy_state {
	IDLE = 1, RX = 2, TX = 3
};

enum mac_internal_event {
	EVENT_TIMEOUT = 0x01,
	EVENT_RX = 0x02,
	EVENT_ASSOCIATE_REQ = 0x04,
	EVENT_DISSOCIATE_REQ = 0x08,
	EVENT_BEACON_TIME = 0x10,
	EVENT_SLOT_TIME = 0x20
};



enum mac_frame_type {
	FRAME_TYPE_BEACON = 0x1, FRAME_TYPE_MGT = 0x2, FRAME_TYPE_DATA = 0x3
};


enum mac_mgt_value {
	MGT_ASSOCIATE = 0x1, MGT_DISSOCIATE = 0x2, MGT_DATA = 0x3
};

#define MGT_TYPE_MASK 0x0F
#define MGT_LENGTH_MASK  0xF0

enum mac_state {
	STATE_IDLE = 0,
	STATE_BEACON_SEARCH = 1,
	STATE_ASSOCIATING = 2,
	STATE_ASSOCIATED = 3,
	STATE_WAITING = 4
};

static inline void hton_s(uint16_t s, uint8_t* d) {
	d[0] = s >> 8;
	d[1] = s & 0xFF;
}
static inline uint16_t ntoh_s(uint8_t* a) {
	return (((uint16_t) a[0]) << 8) + a[1];
}


typedef union {
	uint16_t _align;
	struct {
		//uint8_t dstAddr[2];
		uint8_t srcAddr[2];
		uint8_t dstAddr;
		uint8_t type;
		uint8_t data[MAX_PACKET_LENGTH];
		uint8_t length;
	};
	uint8_t raw[FRAME_HEADER_LENGTH + MAX_PACKET_LENGTH];
} frame_t;

typedef union {
	uint16_t _align;
	struct {
		//uint8_t dstAddr[2];
		uint8_t srcAddr[2];
		uint8_t dstAddr;
		uint8_t type;
		uint8_t beacon_id;
		uint8_t beacon_data[MAX_BEACON_DATA_LENGTH];
		uint8_t length;
	};
	uint8_t raw[FRAME_HEADER_LENGTH + 1 + MAX_BEACON_DATA_LENGTH];
} beacon_t;


#endif /* INC_TDMA_COMMON_H_ */
