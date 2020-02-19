#include "board.h"
#include "display.h"
#include <Nextion.h>
#include <SerialDebug.h>

NexPage page0 = NexPage(0, 0, "page0");

void display_init()
{
  nexInit();

}

