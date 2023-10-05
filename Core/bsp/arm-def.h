
#ifndef INC_ARM_DEF_H_
#define INC_ARM_DEF_H_

#include <stdint.h>

#define CLOCK_CONF_SECOND 1000

typedef uint32_t clock_time_t;

#define CLOCK_LT(a, b)  ((signed long)((a) - (b)) < 0)


#endif /* INC_ARM_DEF_H_ */
