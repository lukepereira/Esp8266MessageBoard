/** Load WLAN credentials from EEPROM */
void loadCredentials() {  
  EEPROM.begin(512);
  for (int i = 0 ; i < 512 ; i++) {
    EEPROM.write(i, '\0');
  }
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
}

/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

bool validateVote(const JsonObject& obj, String ip) {
  int voteCount = obj["voteIP"].measureLength();
  for (int i=0; i < voteCount; i++) {
    // IP already voted
    if (!strcmp(obj["voteIP"][i], ip.c_str() )){
      return false;
    }   
  }
  // New voter
  return true;
}

/*
void escapeHTML(char * raw){
   &raw = str_replace(raw, "<", "&lt;");
}
*/
