#ifndef __DISPLAY_LOGGER_H__
#define __DISPLAY_LOGGER_H__


#include "board.h"
#include "display.h"
#include "display_components.h"
#include <Nextion.h>
#include <SerialDebug.h>


void display_log_append(char *msg);
void display_log_clear(void);

#endif