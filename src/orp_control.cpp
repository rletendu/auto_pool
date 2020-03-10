#include "parameters.h"
#include "orp_control.h"
#include "measures.h"
#include "parameters.h"
#include "config.h"
#include <SerialDebug.h>

enum orp_mode_t orp_mode = ORP_AUTO;
enum orp_control_state_t orp_control_state = ORP_IDLE;

void orp_control_init(void)
{
    printlnA(F("ORP Control Init"));
}

void orp_control_loop(void)
{

}