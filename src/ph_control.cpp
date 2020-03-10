#include "parameters.h"
#include "ph_control.h"
#include "config.h"
#include "measures.h"
#include "parameters.h"
#include <SerialDebug.h>

enum ph_mode_t ph_mode = PH_AUTO;
enum ph_control_state_t ph_control_state = PH_IDLE;

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
	printlnA(F("Ph Control Init"));
}

void ph_control_loop(void)
{


}