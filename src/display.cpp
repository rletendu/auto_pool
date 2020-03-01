#include "board.h"
#include "display.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"
#include "parameters.h"

display_page_t display_page = PAGE_BOOT;

NexPage page_boot = NexPage(PID_BOOT, 0, "boot");
NexPage page_status = NexPage(PID_STATUS, 0, "status");
NexPage page_control = NexPage(PID_CONTROL, 0, "control");
NexPage page_graph = NexPage(PID_GRAPH, 0, "graph");
NexPage page_log = NexPage(PID_LOG, 0, "log");
NexPage page_options = NexPage(PID_OPTIONS, 0, "option");
NexPage page_ota = NexPage(PID_OTA, 0, "OTA");

NexProgressBar ota_progress = NexProgressBar(PID_OTA, 1, "j0");

NexNumber disp_water_temperature = NexNumber(PID_STATUS, CID_STATUS_WATER_TEMP, "x0");
NexNumber disp_ph = NexNumber(PID_STATUS, CID_STATUS_PH, "x1");
NexNumber disp_orp = NexNumber(PID_STATUS, CID_STATUS_ORP, "x2");
NexNumber disp_pressure = NexNumber(PID_STATUS, CID_STATUS_PRESSURE, "x3");
NexNumber disp_sys_temperature = NexNumber(PID_STATUS, CID_STATUS_SYSTEM_TEMP, "x4");
NexNumber disp_sys_humidity = NexNumber(PID_STATUS, CID_STATUS_SYSTEM_HUMIDITY, "x5");
NexPicture disp_wifi = NexPicture(PID_STATUS, CID_STATUS_WIFI, "p0");

NexPicture disp_led_ph = NexPicture(PID_STATUS, CID_STATUS_LED_PH, "p4");
NexPicture disp_led_orp = NexPicture(PID_STATUS, CID_STATUS_LED_ORP, "p5");
NexPicture disp_led_pressure = NexPicture(PID_STATUS, CID_STATUS_LED_PRESSURE, "p6");

NexPicture disp_next_status = NexPicture(PID_STATUS, CID_STATUS_NEXT, "p3");
NexPicture disp_prev_status = NexPicture(PID_STATUS, CID_STATUS_PREV, "p2");

NexPicture disp_next_control = NexPicture(PID_CONTROL, CID_CONTROL_NEXT, "p3");
NexPicture disp_prev_control = NexPicture(PID_CONTROL, CID_CONTROL_PREV, "p2");

NexPicture disp_next_graph = NexPicture(PID_GRAPH, CID_GRAPH_NEXT, "p3");
NexPicture disp_prev_graph = NexPicture(PID_GRAPH, CID_GRAPH_PREV, "p2");

NexPicture disp_next_log = NexPicture(PID_LOG, CID_LOG_NEXT, "p3");
NexPicture disp_prev_log = NexPicture(PID_LOG, CID_LOG_PREV, "p2");

NexButton disp_options_ok = NexButton(PID_OPTIONS, CID_OPTIONS_OK, "b12");
NexButton disp_options_cancel = NexButton(PID_OPTIONS, CID_OPTIONS_CANCEL, "b13");

NexNumber disp_options_delta_ph = NexNumber(PID_OPTIONS, CID_OPTIONS_DELTA_PH, "x0");
NexNumber disp_options_ph = NexNumber(PID_OPTIONS, 1, "x1");
NexNumber disp_options_orp = NexNumber(PID_OPTIONS, CID_OPTIONS_ORP, "x2");
NexNumber disp_options_delta_orp = NexNumber(PID_OPTIONS, CID_OPTIONS_ORP, "x3");
NexNumber disp_options_cl_flow = NexNumber(PID_OPTIONS, CID_OPTIONS_CL_FLOW, "x4");
NexNumber disp_options_ph_minus_flow = NexNumber(PID_OPTIONS, CID_OPTIONS_PH_MINUS_FLOW, "x5");
NexNumber disp_options_ph_plus_flow = NexNumber(PID_OPTIONS, CID_OPTIONS_PH_PLUS_FLOW, "x6");
NexNumber disp_options_pressure_warning = NexNumber(PID_OPTIONS, CID_OPTIONS_PRESSURE, "x7");

NexRadio disp_options_mode_timer_prog = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_TIMER_PROG, "r0");
NexRadio disp_options_mode_fct_t = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_FCT_T, "r1");

NexCheckbox disp_otions_0h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_0H, "c0");

void disp_options_ok_Callback(void *ptr)
{
}

void disp_next_prev_Callback(void *ptr)
{
  NexPicture *btn = (NexPicture *)ptr;
  uint16_t id = (btn->getObjPid() << 8) + btn->getObjCid();

  switch (id)
  {
  case ((PID_STATUS << 8) + CID_STATUS_NEXT):
    display_page = PAGE_CONTROL;
    break;

  case ((PID_STATUS << 8) + CID_STATUS_PREV):
    display_page = PAGE_LOG;
    break;

  case ((PID_CONTROL << 8) + CID_CONTROL_NEXT):
    display_page = PAGE_GRAPH;
    break;

  case ((PID_CONTROL << 8) + CID_CONTROL_PREV):
    display_page = PAGE_STATUS;
    break;

  case ((PID_GRAPH << 8) + CID_GRAPH_NEXT):
    display_page = PAGE_LOG;
    break;

  case ((PID_GRAPH << 8) + CID_GRAPH_PREV):
    display_page = PAGE_CONTROL;
    break;

  case ((PID_LOG << 8) + CID_LOG_NEXT):
    display_page = PAGE_STATUS;
    break;

  case ((PID_LOG << 8) + CID_LOG_PREV):
    display_page = PAGE_GRAPH;
    break;

  default:
    break;
  }
}

/*
  Register object textNumber, buttonPlus, buttonMinus, to the touch event list.
*/
NexTouch *nex_listen_list[] =
    {
        &disp_next_status,
        NULL};

void display_init()
{
  nexInit();
  disp_next_status.attachPush(disp_next_prev_Callback, &disp_next_status);
  disp_prev_status.attachPush(disp_next_prev_Callback, &disp_prev_status);
  disp_next_control.attachPush(disp_next_prev_Callback, &disp_next_control);
  disp_prev_control.attachPush(disp_next_prev_Callback, &disp_next_control);
  disp_next_graph.attachPush(disp_next_prev_Callback, &disp_next_graph);
  disp_prev_graph.attachPush(disp_next_prev_Callback, &disp_prev_graph);
  disp_next_log.attachPush(disp_next_prev_Callback, &disp_next_log);
  disp_prev_log.attachPush(disp_next_prev_Callback, &disp_prev_log);
}

void display_loop(void)
{
  nexLoop(nex_listen_list);
}

void disp_page_ota()
{
  page_ota.show();
}

void disp_ota_progress(uint8_t progress)
{
  ota_progress.setValue(progress);
}

void disp_parameters_to_display(void)
{
  disp_options_ph.setValue(parameters.target_ph);
  disp_options_delta_ph.setValue(parameters.delta_ph);

  disp_options_orp.setValue(parameters.target_redox);
  disp_options_delta_orp.setValue(parameters.delta_redox);

  disp_options_cl_flow.setValue(parameters.flow_cl);
  disp_options_ph_minus_flow.setValue(parameters.flow_ph_minus);
  disp_options_ph_plus_flow.setValue(parameters.flow_ph_plus);

  disp_options_pressure_warning.setValue(parameters.pressure_warning);
  disp_options_ph.setValue(parameters.target_ph);
}

void disp_measures_to_display(void)
{
  disp_water_temperature.setValue(measures.water_temperature * 10);
}