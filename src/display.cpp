#include "board.h"
#include "display.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"
#include "mqtt.h"
#include "parameters.h"

display_page_t display_page = PAGE_STATUS;

NexPage page_boot = NexPage(PID_BOOT, 0, "boot");
NexPage page_status = NexPage(PID_STATUS, 0, "status");
NexPage page_control = NexPage(PID_CONTROL, 0, "control");
NexPage page_graph = NexPage(PID_GRAPH, 0, "graph");
NexPage page_log = NexPage(PID_LOG, 0, "log");
NexPage page_options = NexPage(PID_OPTIONS, 0, "option");
NexPage page_ota = NexPage(PID_OTA, 0, "OTA");

NexProgressBar ota_progress = NexProgressBar(PID_OTA, 1, "j0");

NexXfloat disp_water_temperature = NexXfloat(PID_STATUS, CID_STATUS_WATER_TEMP, "x0");
NexXfloat disp_ph = NexXfloat(PID_STATUS, CID_STATUS_PH, "x1");
NexXfloat disp_orp = NexXfloat(PID_STATUS, CID_STATUS_ORP, "x2");
NexXfloat disp_pressure = NexXfloat(PID_STATUS, CID_STATUS_PRESSURE, "x3");
NexXfloat disp_sys_temperature = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_TEMP, "x4");
NexXfloat disp_sys_humidity = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_HUMIDITY, "x5");
NexPicture disp_wifi = NexPicture(PID_STATUS, CID_STATUS_WIFI, "p0");

NexPicture disp_led_ph = NexPicture(PID_STATUS, CID_STATUS_LED_PH, "p4");
NexPicture disp_led_orp = NexPicture(PID_STATUS, CID_STATUS_LED_ORP, "p5");
NexPicture disp_led_pressure = NexPicture(PID_STATUS, CID_STATUS_LED_PRESSURE, "p6");

NexPicture disp_led_level_water = NexPicture(PID_STATUS, CID_STATUS_WATER_LEVEL, "p7");
NexPicture disp_led_level_cl = NexPicture(PID_STATUS, CID_STATUS_CL_LEVEL, "p8");
NexPicture disp_led_level_ph_minus = NexPicture(PID_STATUS, CID_STATUS_PH_MINUS_LEVEL, "p9");

NexPicture disp_options = NexPicture(PID_STATUS, CID_OPTIONS, "p1");

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

NexXfloat disp_options_delta_ph = NexXfloat(PID_OPTIONS, CID_OPTIONS_DELTA_PH, "x0");
NexXfloat disp_options_ph = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH, "x1");
NexXfloat disp_options_orp = NexXfloat(PID_OPTIONS, CID_OPTIONS_ORP, "x2");
NexXfloat disp_options_delta_orp = NexXfloat(PID_OPTIONS, CID_OPTIONS_ORP, "x3");
NexXfloat disp_options_cl_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_CL_FLOW, "x4");
NexXfloat disp_options_ph_minus_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH_MINUS_FLOW, "x5");
NexXfloat disp_options_ph_plus_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH_PLUS_FLOW, "x6");
NexXfloat disp_options_pressure_warning = NexXfloat(PID_OPTIONS, CID_OPTIONS_PRESSURE, "x7");

NexRadio disp_options_mode_timer_prog = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_TIMER_PROG, "r0");
NexRadio disp_options_mode_fct_t = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_FCT_T, "r1");

NexCheckbox disp_otions_0h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_0H, "c0");
NexCheckbox disp_otions_1h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_1H, "c1");
NexCheckbox disp_otions_2h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_2H, "c2");
NexCheckbox disp_otions_3h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_3H, "c3");
NexCheckbox disp_otions_4h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_4H, "c4");
NexCheckbox disp_otions_5h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_5H, "c5");
NexCheckbox disp_otions_6h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_6H, "c6");
NexCheckbox disp_otions_7h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_7H, "c7");
NexCheckbox disp_otions_8h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_8H, "c8");
NexCheckbox disp_otions_9h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_9H, "c9");
NexCheckbox disp_otions_10h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_10H, "c10");
NexCheckbox disp_otions_11h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_11H, "c11");
NexCheckbox disp_otions_12h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_12H, "c12");
NexCheckbox disp_otions_13h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_13H, "c13");
NexCheckbox disp_otions_14h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_14H, "c14");
NexCheckbox disp_otions_15h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_15H, "c15");
NexCheckbox disp_otions_16h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_16H, "c16");
NexCheckbox disp_otions_17h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_17H, "c17");
NexCheckbox disp_otions_18h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_18H, "c18");
NexCheckbox disp_otions_19h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_19H, "c19");
NexCheckbox disp_otions_20h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_20H, "c20");
NexCheckbox disp_otions_21h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_21H, "c21");
NexCheckbox disp_otions_22h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_22H, "c22");
NexCheckbox disp_otions_23h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_23H, "c23");

NexButton disp_control_filter_auto = NexButton(PID_CONTROL, CID_CONTROL_FILTER_AUTO, "b0");
NexButton disp_control_filter_off = NexButton(PID_CONTROL, CID_CONTROL_FILTER_OFF, "b1");
NexButton disp_control_filter_on = NexButton(PID_CONTROL, CID_CONTROL_FILTER_ON, "b2");

NexButton disp_control_cl_auto = NexButton(PID_CONTROL, CID_CONTROL_CL_AUTO, "b3");
NexButton disp_control_cl_off = NexButton(PID_CONTROL, CID_CONTROL_CL_OFF, "b4");
NexButton disp_control_cl_on = NexButton(PID_CONTROL, CID_CONTROL_CL_ON, "b5");

NexButton disp_control_ph_minus_auto = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_AUTO, "b6");
NexButton disp_control_ph_minus_off = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_OFF, "b7");
NexButton disp_control_ph_minus_on = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_ON, "b8");

NexButton disp_control_ph_plus_auto = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_AUTO, "b9");
NexButton disp_control_ph_plus_off = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_OFF, "b10");
NexButton disp_control_ph_plus_on = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_ON, "b11");

uint32_t disp_timer_prog_to_display(uint32_t timer_prog);
uint32_t disp_disp_to_timer_prog_value(void);
void disp_options_to_parameters(void);

void disp_options_ok_Callback(void *ptr)
{
  printlnA(F("Options OK pressed, reading back updated parameters and save Json"));
  delay(10);
  //page_options.show();
  disp_options_to_parameters();
  page_status.show();
  display_page = PAGE_STATUS;
  parameters_write_json();
  mqtt_publish_parameters();
}

void disp_enter_options_Callback(void *ptr)
{
  display_page = PAGE_OPTIONS;
  disp_parameters_to_display();
}

void disp_next_prev_Callback(void *ptr)
{
  NexPicture *btn = (NexPicture *)ptr;
  uint16_t id = (btn->getObjPid() << 8) + btn->getObjCid();

  switch (id)
  {
  case ((PID_STATUS << 8) + CID_STATUS_NEXT):
  case ((PID_GRAPH << 8) + CID_GRAPH_PREV):
    display_page = PAGE_CONTROL;
    page_control.show();
    printlnA(F("Entering control page"));
    break;

  case ((PID_GRAPH << 8) + CID_GRAPH_NEXT):
  case ((PID_STATUS << 8) + CID_STATUS_PREV):
    display_page = PAGE_LOG;
    page_log.show();
    printlnA(F("Entering log page"));
    break;

  case ((PID_LOG << 8) + CID_LOG_PREV):
  case ((PID_CONTROL << 8) + CID_CONTROL_NEXT):
    display_page = PAGE_GRAPH;
    page_graph.show();
    printlnA(F("Entering Graph page"));
    break;

  case ((PID_LOG << 8) + CID_LOG_NEXT):
  case ((PID_CONTROL << 8) + CID_CONTROL_PREV):
    display_page = PAGE_STATUS;
    page_status.show();
    printlnA(F("Entering Status page"));
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
        &disp_prev_status,
        &disp_next_control,
        &disp_prev_control,
        &disp_next_graph,
        &disp_prev_graph,
        &disp_next_log,
        &disp_prev_log,
        &disp_options_ok,
        &disp_options,
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
  disp_options.attachPush(disp_enter_options_Callback, &disp_options);
  disp_options_ok.attachPush(disp_options_ok_Callback, &disp_options_ok);
  page_boot.show();
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
  disp_options_orp.setValue(parameters.target_orp);
  disp_options_delta_orp.setValue(parameters.delta_orp);
  disp_options_cl_flow.setValue(parameters.flow_cl);
  disp_options_ph_minus_flow.setValue(parameters.flow_ph_minus);
  disp_options_ph_plus_flow.setValue(parameters.flow_ph_plus);
  disp_options_pressure_warning.setValue(parameters.pressure_warning);
  if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
  {
    disp_options_mode_timer_prog.setValue(1);
    disp_options_mode_fct_t.setValue(0);
  }
  else
  {
    disp_options_mode_timer_prog.setValue(0);
    disp_options_mode_fct_t.setValue(1);
  }

  disp_timer_prog_to_display(parameters.timer_prog);
}

void disp_options_to_parameters(void)
{
  float val_f;
  uint32_t val_u;

  printA(F("Reading back pH Target : "));
  disp_options_ph.getValue(&val_f);
  parameters.target_ph = val_f;
  printlnA(parameters.target_ph);
  printA(F("Reading back delta pH Target"));
  disp_options_delta_ph.getValue(&val_f);
  parameters.delta_ph = val_f;
  printlnA(parameters.delta_ph);
  printlnA(F("Reading back ORP Target"));
  disp_options_orp.getValue(&val_f);
  parameters.target_orp = val_f;
  printlnA(F("Reading back ORP delta Target"));
  disp_options_delta_orp.getValue(&val_f);
  parameters.delta_orp = val_f;
  printlnA(F("Reading back cl flow"));
  disp_options_cl_flow.getValue(&val_f);
  parameters.flow_cl = val_f;
  printlnA(F("Reading back pHminus flow"));
  disp_options_ph_minus_flow.getValue(&val_f);
  parameters.flow_ph_minus = val_f;
  printlnA(F("Reading back pHplus flow"));
  disp_options_ph_plus_flow.getValue(&val_f);
  parameters.flow_ph_plus = val_f;
  disp_options_pressure_warning.getValue(&val_f);
  parameters.pressure_warning = val_f;
  disp_options_mode_timer_prog.getValue(&val_u);
  if (val_u)
  {
    parameters.filter_auto_mode = AUTO_TIMER_PROG;
  }
  else
  {
    parameters.filter_auto_mode = AUTO_TIMER_FCT_T;
  }
  parameters.timer_prog = disp_disp_to_timer_prog_value();
}

uint32_t disp_timer_prog_to_display(uint32_t timer_prog)
{
  disp_otions_0h.setValue((timer_prog >> 0) & 0x01);
  disp_otions_1h.setValue((timer_prog >> 1) & 0x01);
  disp_otions_2h.setValue((timer_prog >> 2) & 0x01);
  disp_otions_3h.setValue((timer_prog >> 3) & 0x01);
  disp_otions_4h.setValue((timer_prog >> 4) & 0x01);
  disp_otions_5h.setValue((timer_prog >> 5) & 0x01);
  disp_otions_6h.setValue((timer_prog >> 6) & 0x01);
  disp_otions_7h.setValue((timer_prog >> 7) & 0x01);
  disp_otions_8h.setValue((timer_prog >> 8) & 0x01);
  disp_otions_9h.setValue((timer_prog >> 9) & 0x01);
  disp_otions_10h.setValue((timer_prog >> 10) & 0x01);
  disp_otions_11h.setValue((timer_prog >> 11) & 0x01);
  disp_otions_12h.setValue((timer_prog >> 12) & 0x01);
  disp_otions_13h.setValue((timer_prog >> 13) & 0x01);
  disp_otions_14h.setValue((timer_prog >> 14) & 0x01);
  disp_otions_15h.setValue((timer_prog >> 15) & 0x01);
  disp_otions_16h.setValue((timer_prog >> 16) & 0x01);
  disp_otions_17h.setValue((timer_prog >> 17) & 0x01);
  disp_otions_18h.setValue((timer_prog >> 18) & 0x01);
  disp_otions_19h.setValue((timer_prog >> 19) & 0x01);
  disp_otions_20h.setValue((timer_prog >> 20) & 0x01);
  disp_otions_21h.setValue((timer_prog >> 21) & 0x01);
  disp_otions_22h.setValue((timer_prog >> 22) & 0x01);
  disp_otions_23h.setValue((timer_prog >> 23) & 0x01);
}

uint32_t disp_disp_to_timer_prog_value(void)
{
  uint32_t timer_prog = 0;
  uint32_t cb_val;
  disp_otions_0h.getValue(&cb_val);
  timer_prog += cb_val << 0;
  disp_otions_1h.getValue(&cb_val);
  timer_prog += cb_val << 1;
  disp_otions_2h.getValue(&cb_val);
  timer_prog += cb_val << 2;
  disp_otions_3h.getValue(&cb_val);
  timer_prog += cb_val << 3;
  disp_otions_4h.getValue(&cb_val);
  timer_prog += cb_val << 4;
  disp_otions_5h.getValue(&cb_val);
  timer_prog += cb_val << 5;
  disp_otions_6h.getValue(&cb_val);
  timer_prog += cb_val << 6;
  disp_otions_7h.getValue(&cb_val);
  timer_prog += cb_val << 7;
  disp_otions_8h.getValue(&cb_val);
  timer_prog += cb_val << 8;
  disp_otions_9h.getValue(&cb_val);
  timer_prog += cb_val << 9;
  disp_otions_10h.getValue(&cb_val);
  timer_prog += cb_val << 10;
  disp_otions_11h.getValue(&cb_val);
  timer_prog += cb_val << 11;
  disp_otions_12h.getValue(&cb_val);
  timer_prog += cb_val << 12;
  disp_otions_13h.getValue(&cb_val);
  timer_prog += cb_val << 13;
  disp_otions_14h.getValue(&cb_val);
  timer_prog += cb_val << 14;
  disp_otions_15h.getValue(&cb_val);
  timer_prog += cb_val << 15;
  disp_otions_16h.getValue(&cb_val);
  timer_prog += cb_val << 16;
  disp_otions_17h.getValue(&cb_val);
  timer_prog += cb_val << 17;
  disp_otions_18h.getValue(&cb_val);
  timer_prog += cb_val << 18;
  disp_otions_19h.getValue(&cb_val);
  timer_prog += cb_val << 19;
  disp_otions_20h.getValue(&cb_val);
  timer_prog += cb_val << 20;
  disp_otions_21h.getValue(&cb_val);
  timer_prog += cb_val << 21;
  disp_otions_22h.getValue(&cb_val);
  timer_prog += cb_val << 22;
  disp_otions_23h.getValue(&cb_val);
  timer_prog += cb_val << 23;
  return timer_prog;
}

void disp_measures_to_display(void)
{
  disp_water_temperature.setValue(measures.water_temperature);
  disp_ph.setValue(measures.ph);
  disp_pressure.setValue(measures.pump_pressure);
  disp_orp.setValue(measures.orp);
  disp_sys_humidity.setValue(measures.system_humidity);
  disp_sys_temperature.setValue(measures.system_temperature);
  if (measures.level_cl)
  {
    disp_led_level_cl.setPic(ID_IMAGE_GREEN);
  }
  else
  {
    disp_led_level_cl.setPic(ID_IMAGE_RED);
  }
  if (measures.level_ph_minus)
  {
    disp_led_level_ph_minus.setPic(ID_IMAGE_GREEN);
  }
  else
  {
    disp_led_level_ph_minus.setPic(ID_IMAGE_RED);
  }
  if (measures.level_water)
  {
    disp_led_level_water.setPic(ID_IMAGE_GREEN);
  }
  else
  {
    disp_led_level_water.setPic(ID_IMAGE_RED);
  }
}