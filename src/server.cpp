#include "autopool.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include "ota_tft.h"

const char *host = "autopool";

WebServer server(80);
ota_tft nextion(115200);
extern SoftTimer timer_pool;
bool tft_update_success_action(void *);
uintptr_t tft_update_success_task;
File fsUploadFile;
int fileSize = 0;
bool result = true;

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
	Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
	return false;
}

bool exists(String path)
{
	bool yes = false;
	File file = SPIFFS.open(path, "r");
	if (!file.isDirectory())
	{
		yes = true;
	}
	file.close();
	return yes;
}

void handleFileList()
{
	if (!server.hasArg("dir"))
	{
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}

	String path = server.arg("dir");
	printlnA("handleFileList: " + path);

	File root = SPIFFS.open(path);
	path = String();

	String output = "[";
	if (root.isDirectory())
	{
		File file = root.openNextFile();
		while (file)
		{
			if (output != "[")
			{
				output += ',';
			}
			output += "{\"type\":\"";
			output += (file.isDirectory()) ? "dir" : "file";
			output += "\",\"name\":\"";
			output += String(file.name()).substring(1);
			output += "\"}";
			file = root.openNextFile();
		}
	}
	output += "]";
	server.send(200, "text/json", output);
}
void handleFileUpload()
{
	if (server.uri() != "/edit")
	{
		return;
	}
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		String filename = upload.filename;
		if (!filename.startsWith("/"))
		{
			filename = "/" + filename;
		}
		printA("handleFileUpload Name: ");
		printlnA(filename);
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		//printA("handleFileUpload Data: "); printlnA(upload.currentSize);
		if (fsUploadFile)
		{
			fsUploadFile.write(upload.buf, upload.currentSize);
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		if (fsUploadFile)
		{
			fsUploadFile.close();
		}
		printA("handleFileUpload Size: ");
		printlnA(upload.totalSize);
	}
}

void handleFileDelete()
{
	if (server.args() == 0)
	{
		return server.send(500, "text/plain", "BAD ARGS");
	}
	String path = server.arg(0);
	printlnA("handleFileDelete: " + path);
	if (path == "/")
	{
		return server.send(500, "text/plain", "BAD PATH");
	}
	if (!exists(path))
	{
		return server.send(404, "text/plain", "FileNotFound");
	}
	SPIFFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate()
{
	if (server.args() == 0)
	{
		return server.send(500, "text/plain", "BAD ARGS");
	}
	String path = server.arg(0);
	printlnA("handleFileCreate: " + path);
	if (path == "/")
	{
		return server.send(500, "text/plain", "BAD PATH");
	}
	if (exists(path))
	{
		return server.send(500, "text/plain", "FILE EXISTS");
	}
	File file = SPIFFS.open(path, "w");
	if (file)
	{
		file.close();
	}
	else
	{
		return server.send(500, "text/plain", "CREATE FAILED");
	}
	server.send(200, "text/plain", "");
	path = String();
}

// handle the TFT file uploads
bool handleTftFileUpload()
{
	static bool init_done = false;

	printlnA(F("handleTftFileUpload"));
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
	return true;
}

void webserver_init(void)
{
	printlnA(F("Init Webserver")) if (!SPIFFS.begin())
	{
		return;
	}
	MDNS.begin(host);

	// /settings POST request -> TFT Upload request
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
		handleTftFileUpload);

	// receive fileSize once a file is selected (Workaround as the file content-length is of by +/- 200 bytes. Known issue: https://github.com/esp8266/Arduino/issues/3787)
	server.on("/fs", HTTP_POST, []() {
		fileSize = server.arg(F("fileSize")).toInt();
		server.send(200, F("text/plain"), "");
	});
	// list fs content
	server.on("/list", HTTP_GET, handleFileList);
	server.on("/edit", HTTP_GET, []() {
		if (!handleFileRead("/edit.htm"))
		{
			server.send(404, "text/plain", "FileNotFound");
		}
	});
	//create file
	server.on("/edit", HTTP_PUT, handleFileCreate);
	//delete file
	server.on("/edit", HTTP_DELETE, handleFileDelete);
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on(
		"/edit", HTTP_POST, []() {
			server.send(200, "text/plain", "");
		},
		handleFileUpload);

	server.on("/getmeasures", HTTP_GET, []() {
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/getparameters", HTTP_GET, []() {
		server.send(200, F("text/plain"), parameters_json_string);
	});

	server.on("/setparameters", HTTP_POST, []() {
		char buf[PARAMETERS_JSON_MESSAGE_LEN];
		sprintf(buf, "%s", server.arg("plain").c_str());
		if (parameters_json_to_param(buf))
		{
			if (parameters_write_file())
			{
				printlnA(F("Sucessfully update and saved parameters"));
			}
			else
			{
				printlnA(F("Saving parameters failure"));
			}
		}
		else
		{
			printlnA(F("Parsing parameters failure"));
		}

		server.send(204, F("text/plain"), "");
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

	server.on("/reboot", HTTP_GET, []() {
		server.send(200, F("text/plain"), "Ok reboot in 5 sec");
		tft_update_success_task = timer_pool.in(5 * 1000, tft_update_success_action);
	});

	server.on("/rst_daily_ml_orp", HTTP_GET, []() {
		measures.daily_ml_orp = 0;
		server.send(200, F("text/plain"), "Ok reset daily_ml_orp");
	});

	server.on("/filter_off", HTTP_GET, []() {
		filter_enter_mode(FILTER_OFF);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/filter_on", HTTP_GET, []() {
		filter_enter_mode(FILTER_ON);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/filter_pwr_full", HTTP_GET, []() {
		filter_enter_power_mode(FILTER_POWER_FULL);
		server.send(200, F("text/plain"), measures_json_string);
	});

	server.on("/filter_pwr_reg", HTTP_GET, []() {
		filter_enter_power_mode(FILTER_POWER_REG);
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