#include <Arduino.h>
#include "config.h"



enum orp_mode_t
{
    ORP_OFF = 0,
    ORP_ON = 1,
	ORP_AUTO = 2,
};

extern enum orp_mode_t orp_mode = ORP_AUTO;

void orp_init(void);


