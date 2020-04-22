#include "autopool.h"

#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESPNexUpload.h>

const char *host = "autopool";

const char *index_html = "<!DOCTYPE html>\n<html>\n<head>\n  <meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n  <title>Nextion updater</title>\n</head>\n<body>\n    <h3>Choose .tft file to upload</h3>\n    <form method=\"post\" enctype=\"multipart/form-data\">\n        <input type=\"file\" name=\"name\" onchange=\"valSize(this)\">\n        <input id=\"button\" type=\"submit\" value=\"Upload & Update\" disabled>\n    </form>\n    <script>\n      function valSize(file){\n        // get the selected file size and send it to the ESP\n        var fs = file.files[0].size;\n        var xhttp = new XMLHttpRequest();\n        xhttp.onreadystatechange = function(){\n          if(this.readyState == 4 && this.status == 200){\n            // ESP received fileSize enable the submit button\n            document.getElementById(\"button\").disabled = false;\n          }\n        };\n        xhttp.open(\"POST\", \"/fs\", true);\n        xhttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n        xhttp.send(\"fileSize=\"+fs);\n      }\n    </script>\n    <p>Updating might take a while if you have complex .tft file. Check Nextion display for progress.</p>\n</body>\n</html>\n";
const char *failure_html = "<!DOCTYPE html>\n\t<html lang=\"en\">\n\t<head>\n\t\t<title>Nextion Updater</title>\n\t\t<script>\n\t\t\tfunction getUrlVars() {\n\t\t\t\tvar vars = {};\n\t\t\t\tvar parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {\n\t\t\t\tvars[key] = value;\n\t\t\t\t});\n\t\t\t\treturn vars;\n\t\t\t}\n\n\t\t\tfunction setReason() {\n\t\t\t\tvar reason = getUrlVars()[\"reason\"];\n\t\t\t\tdocument.getElementById(\"reason\").innerHTML = decodeURIComponent(reason);\n\t\t\t}\n\t\t</script>\n\t</head>\n\t<body onLoad=\"setReason()\">\n\t\t<h2>Update failed</h2>\n\t\tReason: <strong><span id=\"reason\"></span></strong>\n\t</body>\n</html>\n";
const char *success_html = "<!DOCTYPE html>\n\t<html lang=\"en\">\n\t<head>\n\t\t<title>Nextion Updater</title>\n\t</head>\n\t<body>\n\t\tUpdate successful\n\t</body>\n</html>\n";

WebServer server(80);
ESPNexUpload nextion(115200);

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
{ // send the right file to the client (if it exists)
	Serial.print("handleFileRead: " + path);
	if (path.endsWith("/"))
		path += "index.html";		   // If a folder is requested, send the index file
	String contentType = getContentType(path); // Get the MIME type
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
	{							    // If the file exists, either as a compressed archive, or normal
		if (SPIFFS.exists(pathWithGz))			    // If there's a compressed version available
			path += ".gz";				    // Use the compressed verion
		File file = SPIFFS.open(path, "r");		    // Open the file
		size_t sent = server.streamFile(file, contentType); // Send it to the client
		file.close();					    // Close the file again
		Serial.println(String("\tSent file: ") + path);
		return true;
	}
	Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
	return false;
}

// handle the file uploads
bool handleFileUpload()
{
	stop_display_tasks();

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
		// Prepare the Nextion display by seting up serial and telling it the file size to expect
		result = nextion.prepareUpload(fileSize);
		if (result)
		{
			//Serial.print(F("Start upload. File size is: "));
			//Serial.print(fileSize);
			//Serial.println(F(" bytes"));
		}
		else
		{
			//Serial.println(nextion.statusMessage + "\n");
			return false;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		// Write the received bytes to the nextion
		result = nextion.upload(upload.buf, upload.currentSize);
		if (result)
		{
			//Serial.print(F("."));
		}
		else
		{
			Serial.println(nextion.statusMessage + "\n");
			return false;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		// End the serial connection to the Nextion and softrest it
		nextion.end();
		//Serial.println("");
		//Serial.println(nextion.statusMessage);
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
	    "/", HTTP_POST, []() {
		    Serial.println(F("Succesfully updated Nextion!\n"));
		    // Redirect the client to the success page after handeling the file upload
		    server.sendHeader(F("Location"), F("/success.html"));
		    server.send(303);
		    return true;
	    },
	    // Receive and save the file
	    handleFileUpload);

	// receive fileSize once a file is selected (Workaround as the file content-length is of by +/- 200 bytes. Known issue: https://github.com/esp8266/Arduino/issues/3787)
	server.on("/fs", HTTP_POST, []() {
		fileSize = server.arg(F("fileSize")).toInt();
		server.send(200, F("text/plain"), "");
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
	server.handleClient();
}
