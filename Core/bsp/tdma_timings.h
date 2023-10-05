/*
 * tdma_timings.h
 *
 *  Created on: 2022. 2. 8.
 *      Author: lucky
 */

#ifndef INC_TDMA_TIMINGS_H_
#define INC_TDMA_TIMINGS_H_

#if 0
// 4khz 이상일시
//#define BEACON_TO_SLOT        3277 // 100ms



// 3khz
//#define BEACON_TO_SLOT        65
#define BEACON_TO_SLOT        3277

#define BEACON_SLOT           0
#define DATA_SLOT_MAX         5
#define CTRL_SLOT             (DATA_SLOT_MAX+1)

#define SLOT_LENGTH_CTRL      3277//3277+450
#define SLOT_LENGTH           3277//3277+450//3277+500//3277 // 4ms

#define TIMEOUT_TIME          3277 // 4ms
//#define TIMEOUT_TIME          10000 // 300ms
#define TIMEOUT_COUNT_MAX     5
//#define SAFETY_TIME           1510//3277 // 2ms

// 1.2Khz 통신일시
//#define SAFETY_TIME         3600

// 4Khz 이상일시	//
//#define SAFETY_TIME           1510

// 3 Khz
#define SAFETY_TIME           1510

#define BEACON_PERIOD         (SLOT_LENGTH*(DATA_SLOT_MAX+2)) // 28ms

//1.2khz 통신
//#define BEACON_OVERHEAD       48-8//480-8//(48-8)

//4khz 이상
//#define BEACON_OVERHEAD       480-8//480-8//(48-8)

////3khz
//#define BEACON_OVERHEAD       48-8//480-8//(48-8)  8ms


#define BEACON_OVERHEAD       480-8//480-8//(48-8)  8ms
#endif

// master 100ms  slave 32768hz
#if 0
#define BEACON_TO_SLOT        3277 // 100ms

#define BEACON_SLOT           0

#define DATA_SLOT_MAX         10

#define CTRL_SLOT             (DATA_SLOT_MAX+1)

#define SLOT_LENGTH_CTRL      3277//3277+450
#define SLOT_LENGTH           3277//3277+450//3277+500//3277 // 4ms

#define TIMEOUT_TIME          3277 // 4ms
//#define TIMEOUT_TIME          10000 // 300ms
#define TIMEOUT_COUNT_MAX     5
//#define SAFETY_TIME           1510//3277 // 2ms
#define SAFETY_TIME           1618//3277 // 2ms

#define BEACON_PERIOD         (SLOT_LENGTH*(DATA_SLOT_MAX+2)) // 28ms

//#define BEACON_OVERHEAD       2//480-8//(48-8)
#define BEACON_OVERHEAD       480-8//480-8//(48-8)

#endif

// master 100ms  slave 32768hz    band 40khz    symbol rate 20kps
#if 0
#define BEACON_TO_SLOT        404//809//1618 // 100ms

#define BEACON_SLOT           0

#define DATA_SLOT_MAX         10

#define CTRL_SLOT             (DATA_SLOT_MAX+1)

#define SLOT_LENGTH_CTRL      404//809//1618//3277+450
#define SLOT_LENGTH           404//809//1618//3277+450//3277+500//3277 // 4ms

#define TIMEOUT_TIME          404//809//1618 // 4ms
//#define TIMEOUT_TIME          10000 // 300ms
#define TIMEOUT_COUNT_MAX     5
//#define SAFETY_TIME           1510//3277 // 2ms
#define SAFETY_TIME           204///409//3277 // 2ms

#define BEACON_PERIOD         (SLOT_LENGTH*(DATA_SLOT_MAX+2)) // 28ms

//#define BEACON_OVERHEAD       2//480-8//(48-8)
#define BEACON_OVERHEAD       2//48-8//480-8//(48-8)

#endif

#if 1 // master 100ms        slave 32768hz    band 40khz    symbol rate 20kps
#define BEACON_TO_SLOT        1618 // 100ms

#define BEACON_SLOT           0

#define DATA_SLOT_MAX         10

#define CTRL_SLOT             (DATA_SLOT_MAX+1)

#define SLOT_LENGTH_CTRL      1618//3277+450
#define SLOT_LENGTH           1618//3277+450//3277+500//3277 // 4ms

#define TIMEOUT_TIME          1618 // 4ms
//#define TIMEOUT_TIME          10000 // 300ms
#define TIMEOUT_COUNT_MAX     5
//#define SAFETY_TIME           1510//3277 // 2ms
#define SAFETY_TIME           809//3277 // 2ms

#define BEACON_PERIOD         (SLOT_LENGTH*(DATA_SLOT_MAX+2)) // 28ms

//#define BEACON_OVERHEAD       2//480-8//(48-8)
#define BEACON_OVERHEAD       24-8//48-8//480-8//(48-8)
#endif

#endif /* INC_TDMA_TIMINGS_H_ */
