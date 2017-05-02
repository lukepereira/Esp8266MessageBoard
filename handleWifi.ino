/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname)+".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleWifi() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>Wifi config</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(softAP_ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
    server.sendContent(
    "</table>"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<p> (For internet, configure port forwarding on router using WLAN IP) </p>"
    "<input type='text' placeholder='network' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "\r\n<br />"
    "<p>Return to the <a href='/board'>home page</a>.</p>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
  );
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      server.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
  } else {
    server.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
  }
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}


/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  Serial.println("wifi save");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  saveCredentials();
  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}

void handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}


/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  server.sendContent(
    "<html><head><title>Hacker Union</title></head><body><center><br><br><table id='hnma"
    "in' border='0' cellpadding='0' cellspacing='0' width='85%' bgcolor='#f6f6ef'><tr><td bgcolor='#ff66"
    "00'><table border='0' cellpadding='0' cellspacing='0' width='100%' style='padding:2px'><tr><td styl"
    "e='width:18px;padding-right:4px'><a href='/about'><img width='18' height='18' style='border:1px #ffffff "
    "solid;background-color:#D7F1FA;'></a></td><td style='line-height:15pt; height:10px;'><span class='p"
    "agetop'><b>&nbsp; Hacker Union &nbsp;&nbsp;</b><a href='/board'>posts</a> | <a href='/wifi'>wifi</a"
    "> | <a href='/about'>about</a> </span></td></tr></table></td></tr><tr><td><table border='1'></td></tr><t"
    "r style='height:20px'></tr><tr><td>Res nolunt diu male administrari &mdash; Things refuse to be mis"
    "managed long</td></tr><tr style='height:20px'></tr><tr><td>Stewardship is an ethic that embodies th"
    "e responsible planning and management of resources. The concepts of stewardship can be applied to t"
    "he environment, economics, health, property, and information.</td></tr><tr style='height:20px'></tr"
    "><tr><td>Industrial democracy refers to the organization model in which workplaces are run directly"
    " by the people who work in them in place of private or state ownership of the means of production. "
    "Deliberative democracy is a form of democracy where deliberation is central to decision-making whic"
    "h differs from traditional democratic theory in that authentic deliberation, not mere voting, is th"
    "e primary source of legitimacy for a law.</td></tr><tr style='height:20px'></tr><tr><td>There are n"
    "o political solutions, only technological ones; the rest is propaganda. &mdash; Jacques Ellul</td><"
    "/tr><tr style='height:20px'><tr><td><b>About The Prototype</b></td></tr><tr><td>Hacker Union is an "
    "open wifi network used for group decision-making and communication within a local area. This web se"
    "rver is being hosted on a small and portable wifi thumb drive powered by battery or USB. Posts are "
    "submitted and voted on. The highest ranking proposition is then group edited and made into a cohere"
    "nt and concise legislation. The module shares and collates content with other nearby HU wifi chips,"
    " forming a decentralized mesh network. Connection to the internet is possible given an SSID, passwo"
    "rd, and port-forwarding access on a router. With an internet connection, the module will upload and"
    " backup all content to the internet.</td></tr><tr style='height:20px'></tr></table></form></td></tr"
    "></table><br><br>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p><small>You are connected through the soft AP: ") + softAP_ssid + "</small></p>");
  } else {
    server.sendContent(String("<p><small>You are connected through the wifi network: ") + ssid + "</small></p>");
  }
  server.sendContent(
  "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}


