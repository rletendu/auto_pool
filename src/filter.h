#include <Arduino.h>
#include "config.h"



enum filter_mode_t
{
    FILTER_OFF = 0,
    FILTER_ON = 1,
	FILTER_AUTO = 2,
};

extern enum filter_mode_t filter_mode = FILTER_AUTO;

void filter_init(void);


