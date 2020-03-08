
#ifndef __DISPLAY_H__
#define __DISPLAY_H__


enum display_page_t
{
    PAGE_BOOT,
    PAGE_STATUS,
    PAGE_CONTROL,
    PAGE_GRAPH,
    PAGE_LOG,
    PAGE_OPTIONS,
    PAGE_OTA
};

void display_init(void);
void display_loop(void);

void disp_page_ota();
void disp_ota_progress(uint8_t progress);
void disp_parameters_to_display(void);

#endif