#include "parameters.h"
#include "ph_control.h"
#include <SerialDebug.h>
#include "config.h"
#include "measures.h"
#include "parameters.h"

enum ph_mode_t ph_mode = PH_AUTO;
enum ph_control_state_t ph_control_state = PH_IDLE;

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
}

void ph_control_loop(void)
{


}