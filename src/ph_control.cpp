#include "parameters.h"
#include "ph_control.h"
#include "config.h"
#include "measures.h"
#include "parameters.h"
#include <SerialDebug.h>
#include "display_ctrl.h"


enum ph_control_state_t ph_control_state = PH_IDLE;

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
	printlnA(F("Ph Control Init"));
	control_ph_minus_auto();
    control_ph_plus_auto();
}

void ph_control_loop(void)
{


}