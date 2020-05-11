#include "autopool.h"
#include "telnet.h"
#include <DNSServer.h>
#include "ESPmDNS.h"

RemoteDebug Debug;

void telnet_init(void)
{
	MDNS.begin(HOST_NAME);
	MDNS.addService("telnet", "tcp", 23);
	Debug.begin(HOST_NAME);
	Debug.setResetCmdEnabled(true);
}

void telnet_loop(void)
{
	if (wifi_is_available())
	{
		Debug.handle();
	}
}
