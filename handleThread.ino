void handleThread() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  strcpy(table, "<html><head><title>Hacker Union</title>"    
    "</head><body><center><br><br><table id='hnmain' border='0' cellpadding='0' cellspacing='0' width='85%'" 
    "bgcolor='#f6f6ef'><tr><td bgcolor='#ff6600'>"
    "<table border='0' cellpadding='0' cellspacing='0' width='100%' style='padding:2px'>"
    "<tr><td style='width:18px;padding-right:4px'>"
    "<a href='/about'><img width='18' height='18' style='border:1px #ffffff solid;background-color:#D7F1FA;'></a>"
    "</td><td style='line-height:15pt; height:10px;'><span class='pagetop'><b>&nbsp; Hacker Union &nbsp;&nbsp;</b>"
    "<a href='/board'>posts</a> | <a href='/wifi'>wifi</a> | <a href='/about'>about</a> "
    "</span></td></tr></table></td></tr><tr><td><table border='1' width='100%' bordercolor='#f60'></td></tr>" 
    "<tr style='height:20px'></tr>"
  );
  
  server.sendContent(String(table));

  const char* threadIDBuffer;
  const char* titleBuffer;
  const char* contentBuffer;
  const char* ipBuffer;
  int replyCount;
  int voteCount;
  char *json = (char*)malloc(2000);
  Serial.print("Thread "); Serial.println(server.arg("id"));
  WiFiClient client = server.client();
  File fp = SPIFFS.open(server.arg("id"), "r"); 
  
  if (fp.available()){
    
    int eol = fp.readBytesUntil('\0', json, 2000); // Read JSON string into array
    json[eol] = '\0';
    StaticJsonBuffer<200> jsonBuffer;  // Reserve memory space
    JsonObject& root = jsonBuffer.parseObject(json); // Deserialize the JSON string
    if (!root.success()){
      Serial.println("parseObject() failed");
      return;
    }
    threadIDBuffer = root["threadID"];
    ipBuffer = root["ip"];
    titleBuffer = root["title"];
    contentBuffer = root["body"];
    replyCount = root["replyCount"];
    voteCount = root["voteCount"];
    JsonObject& replies = root["replies"];
    //JsonArray& voteIP = root["voteIP"];

    server.sendContent(String() + "<tr style='height:20px'></tr><tr><td><form method='POST' action='threadsave' style='display:inline;'>");
    server.sendContent(String() + "<input type='hidden' name='id' value='/data/" + String(threadIDBuffer) + ".txt'</input>"); 
    server.sendContent(String() + "<input type='hidden' name='ip' value='" + toStringIp(client.remoteIP()) + "'></input>");
    server.sendContent(String() + "<input type='hidden' name='state' value='vote'></input>");
    server.sendContent(String() + "<input type='submit' value='+'/></form>&emsp;&emsp;&emsp;Thread #" + String(threadIDBuffer));
    server.sendContent(String() + "&emsp;&emsp;&emsp;"+ String(ipBuffer) + "</td></tr><tr><td><b>" + String(titleBuffer));
    server.sendContent(String() + "</b><br><br>" + String(contentBuffer) + "</td></tr><tr style='height:20px'></tr>");

    for(JsonObject::iterator it=replies.begin(); it!=replies.end(); ++it) {
      // *it contains the key/value pair
      const char* replyIP = it->key;
  
      // it->value contains the JsonVariant which can be casted as usual
      const char* reply = it->value;
      server.sendContent(
        "<tr><td>" + String(replyIP) + "</td></tr><tr><td>" + String(reply) + "</td></tr>"
      );
    }
  }

  /*while(fp.readBytesUntil('\0', json, 2000)){ // Read JSON replies into array
    StaticJsonBuffer<2000> jsonBuffer;  // Reserve memory space
    JsonObject& reply = jsonBuffer.parseObject(json); // Deserialize the JSON string
    if (!reply.success()){
      Serial.println("parseObject() failed");
      return;
    }
  }*/
  fp.close();
  server.sendContent(
    "<tr style='height:20px'></tr></table></form></td></tr></table>"
    "<br /><form method='POST' action='threadsave' ><h4>Reply</h4>"
    "<input type='hidden' name='id' value='/data/" + String(threadIDBuffer) + ".txt' </input>"
    "<input type='hidden' name='ip' value='" + String(ipBuffer) +"'></input>"
    "<input type='hidden' name='state' value='reply'></input>"
    "<textarea style='margin: 0px; height: 175px; width: 635px;' placeholder='reply' maxlength='800' name='r' required></textarea>"
    "<br><br><input type='submit' value='Submit' /></form>"
  ); 
  server.client().stop(); // Stop is needed because we sent no content length
  free(json);
}

void handleThreadSave(){
  Serial.println("Thread save");
  Serial.println(server.arg("id"));
  File fpRead = SPIFFS.open(server.arg("id"), "r"); // file titled with threadID
  char *json = (char*)malloc(2000);
  if (!fpRead) {
      Serial.println("file open failed");
      return;
  }
  int eol = fpRead.readBytesUntil('\0', json, 2000); // Read JSON string into array
  json[eol] = '\0';
  StaticJsonBuffer<200> jsonBuffer;  // Reserve memory space
  JsonObject& root = jsonBuffer.parseObject(json); // Deserialize the JSON string
  if (!root.success()){
    Serial.println("parseObject() failed");
    return;
  }
  fpRead.close();
  root.printTo(Serial);
  File fp = SPIFFS.open(server.arg("id"), "w"); // reopen file to overwrite
  if (!fp) {
      Serial.println("file open failed");
      return;
  }
 
  // If thread is being upvoted 
   if (server.arg("state").equals("vote")) { 
   if (validateVote(root, server.arg("ip"))) {
      
        int voteCount = root["voteCount"];
        //root["voteIP"].add(server.arg("ip"));
        root["voteCount"] = voteCount + 1;
      
    }
  // If thread is being replied to
  } else {
    
    int replyCount = root["replyCount"];
    root["replyCount"] = replyCount + 1;
    //JsonArray& replies = root["replies"];
    root["replies"][server.arg("ip")] = server.arg("r");
   
  }
  root.printTo(fp);
  root.printTo(Serial);
  fp.print('\0');
  fp.close();
  //server.send(server.arg("id")); 
  
  server.sendHeader("Location", "board", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  Serial.println("after sendHeader");
  free(json);
}

