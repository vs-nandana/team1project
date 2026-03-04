/*
 * sharedmemory.h
 *
 *  Created on: 27-Feb-2026
 *      Author: nandana
 */

#ifndef INC_SHAREDMEMORY_H_
#define INC_SHAREDMEMORY_H_

#pragma once
#include <stdint.h>

typedef struct {
    volatile float    max;
    volatile float    min;
    volatile float    avg;
    volatile uint32_t sample_rate_index;
    volatile uint32_t count;           // incremented by CM7 each update
    volatile uint8_t  cm7_updated;     // CM7 sets this after writing
    volatile uint8_t  cm4_ack;         // CM4 sets this after reading
    volatile uint8_t is_run;
    volatile uint8_t instant_tx;
    volatile uint8_t reset_stats;
    volatile uint8_t  reserved[3];

} SharedMemory_t;


#endif /* INC_SHAREDMEMORY_H_ */
