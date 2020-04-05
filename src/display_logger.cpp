#include "board.h"
#include "display.h"
#include "display_components.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"

char old_log_content[800];
char new_log_content[800];
uint8_t cr_table[10];

void log_read(void)
{
  disp_log_logger.getText(old_log_content, sizeof(old_log_content));
}

void log_append(char *line)
{
  uint32_t i, j = 0;
  log_read();
  for (i = 0; i < strlen(old_log_content); i++)
  {
    if (old_log_content[i] == '\n')
    {
      cr_table[j] = i;
      j++;
    }
  }
  if (j)
  {
    strcpy(new_log_content, old_log_content + cr_table[0] + 1);
  }
  else
  {
    strcpy(new_log_content, old_log_content);
  }
  strcat(new_log_content, line);
  disp_log_logger.setText(new_log_content);
}
