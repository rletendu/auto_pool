#include "board.h"
#include "display.h"
#include "display_components.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"
#include "mqtt.h"
#include "parameters.h"



void disp_control_ph_plus_auto_Callback(void *ptr)
{

}
void disp_control_ph_plus_on_Callback(void *ptr)
{
}
void disp_control_ph_plus_off_Callback(void *ptr)
{
}
void disp_control_ph_minus_auto_Callback(void *ptr)
{
}
void disp_control_ph_minus_on_Callback(void *ptr)
{
}
void disp_control_ph_minus_off_Callback(void *ptr)
{
}
void disp_control_cl_auto_Callback(void *ptr)
{
}
void disp_control_cl_on_Callback(void *ptr)
{
}
void disp_control_cl_off_Callback(void *ptr)
{
}
void disp_control_filter_auto_Callback(void *ptr)
{
}
void disp_control_filter_on_Callback(void *ptr)
{
}
void disp_control_filter_off_Callback(void *ptr)
{
}


void control_ph_plus_auto(void)
{
  disp_control_ph_plus_auto.Set_background_color_bco(COLOR_GREEN);
  disp_control_ph_plus_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_plus_on.Set_background_color_bco(COLOR_GRAY);
}
void control_ph_plus_on(void)
{
  disp_control_ph_plus_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_plus_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_plus_on.Set_background_color_bco(COLOR_ORANGE);
}
void control_ph_plus_off(void)
{
  disp_control_ph_plus_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_plus_off.Set_background_color_bco(COLOR_RED);
  disp_control_ph_plus_on.Set_background_color_bco(COLOR_GRAY);
}
void control_ph_minus_auto(void)
{
  disp_control_ph_minus_auto.Set_background_color_bco(COLOR_GREEN);
  disp_control_ph_minus_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_minus_on.Set_background_color_bco(COLOR_GRAY);
}
void control_ph_minus_on(void)
{
  disp_control_ph_minus_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_minus_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_minus_on.Set_background_color_bco(COLOR_ORANGE);
}
void control_ph_minus_off(void)
{
  disp_control_ph_minus_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_ph_minus_off.Set_background_color_bco(COLOR_RED);
  disp_control_ph_minus_on.Set_background_color_bco(COLOR_GRAY);
}
void control_cl_auto(void)
{
  disp_control_cl_auto.Set_background_color_bco(COLOR_GREEN);
  disp_control_cl_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_cl_on.Set_background_color_bco(COLOR_GRAY);
}
void control_cl_on(void)
{
  disp_control_cl_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_cl_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_cl_on.Set_background_color_bco(COLOR_ORANGE);
}
void control_cl_off(void)
{
  disp_control_cl_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_cl_off.Set_background_color_bco(COLOR_RED);
  disp_control_cl_on.Set_background_color_bco(COLOR_GRAY);
}
void control_filter_auto(void)
{
  disp_control_filter_auto.Set_background_color_bco(COLOR_GREEN);
  disp_control_filter_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_filter_on.Set_background_color_bco(COLOR_GRAY);
}
void control_filter_on_(void)
{
  disp_control_filter_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_filter_off.Set_background_color_bco(COLOR_GRAY);
  disp_control_filter_on.Set_background_color_bco(COLOR_ORANGE);
}
void control_filter_off(void)
{
  disp_control_filter_auto.Set_background_color_bco(COLOR_GRAY);
  disp_control_filter_off.Set_background_color_bco(COLOR_RED);
  disp_control_filter_on.Set_background_color_bco(COLOR_GRAY);
}
