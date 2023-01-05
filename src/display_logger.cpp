#include "autopool.h"

char log_history[DISP_LOG_NB_LINES][DISP_LOG_NB_CHAR_PER_LINE];
char disp_log_content[DISP_LOG_NB_LINES * (DISP_LOG_NB_CHAR_PER_LINE + 2)];

void display_log_clear(void)
{
	disp_log_logger.setText("");
	for (int i = 0; i < DISP_LOG_NB_LINES; i++)
	{
		log_history[i][0] = 0;
	}
}

void display_log_append(const char *msg)
{
	static int write_index = 0;
	static int read_index;
	static int nb_entries = 0;
	int visible_entries;
	char timestamp[20];

	sprintf(timestamp, "%02u:%02u:%02u : ", rtc_get_hour(), rtc_get_minute(), rtc_get_second());

	if (nb_entries)
	{
		if (++write_index >= DISP_LOG_NB_LINES)
		{
			write_index = 0;
		}
	}
	strcpy(log_history[write_index], timestamp);
	strcat(log_history[write_index], msg);
	if (nb_entries < DISP_LOG_NB_LINES)
	{
		nb_entries++;
	}

	if (nb_entries >= DISP_LOG_NB_VISIBLE_LINES)
	{
		visible_entries = DISP_LOG_NB_VISIBLE_LINES;
	}
	else
	{
		visible_entries = nb_entries;
	}

	read_index = write_index - visible_entries + 1;
	if (read_index < 0)
	{
		read_index = DISP_LOG_NB_LINES + read_index;
	}

	disp_log_content[0] = 0;
	for (int i = 0; i < visible_entries; i++)
	{
		strcat(disp_log_content, log_history[read_index]);
		strcat(disp_log_content, "\r\n");
		if (++read_index >= DISP_LOG_NB_LINES)
		{
			read_index = 0;
		}
	}
	disp_log_logger.setText(disp_log_content);
}