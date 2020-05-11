#include "autopool.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include "ota_tft.h"

const char *host = "autopool";

const char *index_html = "<!DOCTYPE html>\n<html>\n<head>\n  <meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n  <title>Nextion updater</title>\n</head>\n<body>\n    <h3>Choose .tft file to upload</h3>\n    <form method=\"post\" enctype=\"multipart/form-data\">\n        <input type=\"file\" name=\"name\" onchange=\"valSize(this)\">\n        <input id=\"button\" type=\"submit\" value=\"Upload & Update\" disabled>\n    </form>\n    <script>\n      function valSize(file){\n        // get the selected file size and send it to the ESP\n        var fs = file.files[0].size;\n        var xhttp = new XMLHttpRequest();\n        xhttp.onreadystatechange = function(){\n          if(this.readyState == 4 && this.status == 200){\n            // ESP received fileSize enable the submit button\n            document.getElementById(\"button\").disabled = false;\n          }\n        };\n        xhttp.open(\"POST\", \"/fs\", true);\n        xhttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n        xhttp.send(\"fileSize=\"+fs);\n      }\n    </script>\n    <p>Updating might take a while if you have complex .tft file. Check Nextion display for progress.</p>\n</body>\n</html>\n";
const char *failure_html = "<!DOCTYPE html>\n\t<html lang=\"en\">\n\t<head>\n\t\t<title>Nextion Updater</title>\n\t\t<script>\n\t\t\tfunction getUrlVars() {\n\t\t\t\tvar vars = {};\n\t\t\t\tvar parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {\n\t\t\t\tvars[key] = value;\n\t\t\t\t});\n\t\t\t\treturn vars;\n\t\t\t}\n\n\t\t\tfunction setReason() {\n\t\t\t\tvar reason = getUrlVars()[\"reason\"];\n\t\t\t\tdocument.getElementById(\"reason\").innerHTML = decodeURIComponent(reason);\n\t\t\t}\n\t\t</script>\n\t</head>\n\t<body onLoad=\"setReason()\">\n\t\t<h2>Update failed</h2>\n\t\tReason: <strong><span id=\"reason\"></span></strong>\n\t</body>\n</html>\n";
const char *success_html = "<!DOCTYPE html>\n\t<html lang=\"en\">\n\t<head>\n\t\t<title>Nextion Updater</title>\n\t</head>\n\t<body>\n\t\tUpdate successful\n\t</body>\n</html>\n";

WebServer server(80);
ota_tft nextion(115200);
extern SoftTimer timer_pool;
bool tft_update_success_action(void *);
uintptr_t tft_update_success_task;

int fileSize = 0;
bool result = true;

String getJsPage(float val)
{
	char buf[10];
	String page = "document.write(\"";
	sprintf(buf, "%.1f", val);
	page += buf;
	page += "\");";
	return page;
}

String getContentType(String filename)
{
	if (server.hasArg(F("download")))
		return F("application/octet-stream");
	else if (filename.endsWith(F(".htm")))
		return F("text/html");
	else if (filename.endsWith(".html"))
		return F("text/html");
	else if (filename.endsWith(F(".css")))
		return F("text/css");
	else if (filename.endsWith(F(".js")))
		return F("application/javascript");
	else if (filename.endsWith(F(".png")))
		return F("image/png");
	else if (filename.endsWith(F(".gif")))
		return F("image/gif");
	else if (filename.endsWith(F(".jpg")))
		return F("image/jpeg");
	else if (filename.endsWith(F(".ico")))
		return F("image/x-icon");
	else if (filename.endsWith(F(".xml")))
		return F("text/xml");
	else if (filename.endsWith(F(".pdf")))
		return F("application/x-pdf");
	else if (filename.endsWith(F(".zip")))
		return F("application/x-zip");
	else if (filename.endsWith(F(".gz")))
		return F("application/x-gzip");
	return F("text/plain");
}

bool handleFileRead(String path)
{
	// send the right file to the client (if it exists)
	Serial.print("handleFileRead: " + path);
	if (path.endsWith("/"))
		path += "index.html";				   // If a folder is requested, send the index file
	String contentType = getContentType(path); // Get the MIME type
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
	{														// If the file exists, either as a compressed archive, or normal
		if (SPIFFS.exists(pathWithGz))						// If there's a compressed version available
			path += ".gz";									// Use the compressed verion
		File file = SPIFFS.open(path, "r");					// Open the file
		size_t sent = server.streamFile(file, contentType); // Send it to the client
		file.close();										// Close the file again
		Serial.println(String("\tSent file: ") + path);
		return true;
	}
	if (path == "/system_temperature.js")
	{
		server.send(200, "text/html", getJsPage(measures.system_temperature));
		return true;
	}
	else if (path == "/system_humidity.js")
	{
		server.send(200, "text/html", getJsPage(measures.system_humidity));
		return true;
	}
	else if (path == "/water_temperature.js")
	{
		server.send(200, "text/html", getJsPage(measures.water_temperature));
		return true;
	}
	else if (path == "/orp.js")
	{
		server.send(200, "text/html", getJsPage((float)measures.orp));
		return true;
	}
	else if (path == "/ph.js")
	{
		server.send(200, "text/html", getJsPage(measures.ph));
		return true;
	}
	else if (path == "/pump_pressure.js")
	{
		server.send(200, "text/html", getJsPage(measures.pump_pressure));
		return true;
	}
	Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
	return false;
}

// handle the file uploads
bool handleFileUpload()
{
	static bool init_done = false;

	printlnA(F("handleFileUpload"));
	if (init_done == false)
	{

		init_done = true;
	}
	HTTPUpload &upload = server.upload();

	// Check if file seems valid nextion tft file
	if (!upload.filename.endsWith(F(".tft")))
	{
		server.send(500, F("text/plain"), F("ONLY TFT FILES ALLOWED\n"));
		return false;
	}

	if (!result)
	{
		// Redirect the client to the failure page
		server.sendHeader(F("Location"), "/failure.html?reason=" + nextion.statusMessage);
		server.send(303);
		return false;
	}

	if (upload.status == UPLOAD_FILE_START)
	{
		beep(10);
		stop_display_tasks();
		printlnA(F("UPLOAD_FILE_START Preparing nextion"));

		// Prepare the Nextion display by seting up serial and telling it the file size to expect
		result = nextion.prepareUpload(fileSize);
		printlnA(F("Nextion prepare Done"));

		if (result)
		{
			printA(F("Nextion Prepare pass Start upload. File size is: "));
			printA(fileSize);
			printlnA(F(" bytes"));
			delay(500);
		}
		else
		{
			printA(F("Nextion prepage fail: "));
			printlnA(nextion.statusMessage + "\n");
			return false;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		printA("UPLOAD_FILE_WRITE: ");
		// Write the received bytes to the nextion
		result = nextion.upload(upload.buf, upload.currentSize);
		if (result)
		{
			printlnA(F("ok"));
		}
		else
		{
			printlnA(nextion.statusMessage);
			return false;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		// End the serial connection to the Nextion and softrest it
		nextion.end();
		printA("UPLOAD_FILE_END");
		printlnA(nextion.statusMessage);
		return true;
	}
}

void webserver_init(void)
{
	if (!SPIFFS.begin())
	{
		return;
	}

	MDNS.begin(host);

	//SERVER INIT
	server.on(
		"/settings.html", HTTP_POST, []() {
			Serial.println(F("Succesfully updated Nextion!\n"));
			// Redirect the client to the success page after handeling the file upload
			server.sendHeader(F("Location"), F("/success.html"));
			server.send(303);
			tft_update_success_task = timer_pool.in(5 * 1000, tft_update_success_action);
			return true;
		},
		// Receive and save the file
		handleFileUpload);

	// receive fileSize once a file is selected (Workaround as the file content-length is of by +/- 200 bytes. Known issue: https://github.com/esp8266/Arduino/issues/3787)
	server.on("/fs", HTTP_POST, []() {
		fileSize = server.arg(F("fileSize")).toInt();
		server.send(200, F("text/plain"), "");
	});

	server.on("/getmeasures", HTTP_GET, []() {
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/getfilterstate", HTTP_GET, []() {
		server.send(200, F("text/plain"), state_filter_json_string);
	});

	server.on("/getorpstate", HTTP_GET, []() {
		server.send(200, F("text/plain"), state_orp_json_string);
	});

	server.on("/getphstate", HTTP_GET, []() {
		server.send(200, F("text/plain"), state_ph_json_string);
	});

	server.on("/filter_auto", HTTP_GET, []() {
		filter_enter_mode(FILTER_AUTO);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/filter_off", HTTP_GET, []() {
		filter_enter_mode(FILTER_OFF);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/filter_on", HTTP_GET, []() {
		filter_enter_mode(FILTER_ON);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/orp_auto", HTTP_GET, []() {
		orp_enter_mode(ORP_AUTO);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/orp_off", HTTP_GET, []() {
		orp_enter_mode(ORP_OFF);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/orp_on", HTTP_GET, []() {
		orp_enter_mode(ORP_ON);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/ph_auto", HTTP_GET, []() {
		ph_minus_enter_mode(PH_MINUS_AUTO);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/ph_off", HTTP_GET, []() {
		ph_minus_enter_mode(PH_MINUS_OFF);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/ph_on", HTTP_GET, []() {
		ph_minus_enter_mode(PH_MINUS_ON);
		server.send(200, F("text/plain"), measures_json_string);
	});
	// called when the url is not defined here
	// use it to load content from SPIFFS
	server.onNotFound([]() {
		if (!handleFileRead(server.uri()))
			server.send(404, F("text/plain"), F("FileNotFound"));
	});

	server.begin();
}

void webserver_loop(void)
{
	if (wifi_is_available())
	{
		server.handleClient();
	}
}

bool tft_update_success_action(void *)
{
	esp_restart();
}

void webserver_stop(void)
{
	MDNS.end();
	server.stop();
}