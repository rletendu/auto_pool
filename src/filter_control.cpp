#include "parameters.h"
#include "filter_control.h"
#include <SerialDebug.h>
#include "config.h"
#include "measures.h"
#include "parameters.h"
#include "timer.h"
#include "display_ctrl.h"

enum filter_mode_t filter_mode = FILTER_AUTO;




void filter_control_init(void)
{
	printlnA(F("Filter Control Init"));
	control_filter_auto();
}

void filter_control_loop(void)
{

}