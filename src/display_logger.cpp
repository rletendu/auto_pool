#include "board.h"
#include "display.h"
#include "display_components.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"

char old_log_content[DISP_LOG_NB_CHAR];
char new_log_content[DISP_LOG_NB_CHAR];

static void log_read(void)
{
	disp_log_logger.getText(old_log_content, sizeof(old_log_content));
}

void display_log_clear(void)
{
	disp_log_logger.setText("");
}

void display_log_append(char *msg)
{
	uint16_t cr_table[DISP_LOG_NB_LINES+1];
	char timestamp[20];
	sprintf(timestamp, "%02u:%02u:%02u : ", rtc_get_hour(), rtc_get_minute(), rtc_get_second());
	uint16_t i;
	uint16_t cr_cnt = 0;
	log_read();
	for (i = 0; i < strlen(old_log_content); i++)
	{
		if (old_log_content[i] == '\r')
		{
			cr_table[cr_cnt] = i;
			cr_cnt++;
		}
	}
	if (cr_cnt >= DISP_LOG_NB_LINES)
	{
		strcpy(new_log_content, old_log_content + cr_table[0] + 2);
	}
	else
	{
		strcpy(new_log_content, old_log_content);
	}
	strcat(new_log_content, timestamp);
	strcat(new_log_content, msg);
	strcat(new_log_content, "\r\n");
	disp_log_logger.setText(new_log_content);
}
