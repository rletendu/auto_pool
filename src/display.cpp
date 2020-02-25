#include "board.h"
#include "display.h"
#include <Nextion.h>
#include <SerialDebug.h>

NexPage page_boot = NexPage(0, 0, "boot");
NexPage page_status = NexPage(0, 0, "status");
NexPage page_control = NexPage(0, 0, "control");
NexPage page_graph = NexPage(0, 0, "graph");
NexPage page_log = NexPage(0, 0, "log");
NexPage page_options = NexPage(0, 0, "option");
NexPage page_ota = NexPage(0, 0, "OTA");

NexProgressBar ota_progress = NexProgressBar(0, 1, "j0");

void display_init()
{
  nexInit();
}

void disp_page_ota()
{
  page_ota.show();
}
void disp_ota_progress(uint8_t progress)
{
  ota_progress.setValue(progress);
}
