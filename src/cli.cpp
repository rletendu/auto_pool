#include "autopool.h"
#include <SimpleCLI.h>


SimpleCLI cli;

// Commands
Command cmd_portal;
Command cmd_portal_reset;
Command cmd_format;

void cmd_portalCallback(cmd *c)
{
	Command cmd(c);
	printlnA(F("Portal request"));
	wifimanager_start_portal();
	printlnA(F("Rebooting..."));
	ESP.restart();
}

void cmd_portalResetCallback(cmd *c)
{
	Command cmd(c);
	printlnA(F("Portal reset"));
	wifimanager_reset_portal();
	printlnA(F("Rebooting..."));
	ESP.restart();
}

void cmd_FormatCallback(cmd *c)
{
	printlnA(F("Format SPIFF..."));
	SPIFFS.format();
}

void cli_init(void)
{
	cmd_portal = cli.addSingleArgCmd("portal", cmd_portalCallback);
	cmd_portal_reset = cli.addSingleArgCmd("portal_reset", cmd_portalResetCallback);
	cmd_format = cli.addSingleArgCmd("format", cmd_FormatCallback);
}

void cli_loop(void)
{
	if (Serial.available())
	{
		String input = Serial.readStringUntil('\n');
		cli.parse(input);
	}
}