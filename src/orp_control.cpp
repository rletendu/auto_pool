#include "parameters.h"
#include "orp_control.h"
#include "measures.h"
#include "parameters.h"
#include "config.h"
#include <SerialDebug.h>
#include "display_ctrl.h"


enum orp_control_state_t orp_control_state = ORP_IDLE;

void orp_control_init(void)
{
    printlnA(F("ORP Control Init"));
    control_cl_auto();

}

void orp_control_loop(void)
{

}