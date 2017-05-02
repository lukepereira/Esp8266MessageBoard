void handleBoard() {
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
    "<tr style='height:20px'></tr>");
  server.sendContent(String(table));

  Serial.println("Handle Board");
  /*
   * JsonArray of N element 8 + 12 * N
   * JsonObject of N element 8 + 16 * N
   */
  StaticJsonBuffer<200> jsonBuffer;  // Reserve memory space
  const char* threadIDBuffer;
  const char* ipBuffer;
  const char* titleBuffer;
  int replyCount;
  int voteCount;
  Dir dir = SPIFFS.openDir("/data");
  
  // Iterate through thread files 
  while (dir.next()) {
    char fileName[30];
    dir.fileName().toCharArray(fileName,30);
    Serial.print("dir = " ); Serial.println(dir.fileName());
    
    File fp = dir.openFile("r");
    if (!fp) {
      Serial.println("file open failed");
      break;
    }
    char *json = (char*)malloc(2000);
    int eol = fp.readBytesUntil('\0', json, 2000); // Read JSON string into array
    json[eol] = '\0';
    Serial.println(json);
    JsonObject& root = jsonBuffer.parseObject(json); // Deserialize the JSON string
    if (!root.success()){
      Serial.println("parseObject() failed");
      return;
    }
    threadIDBuffer = root["threadID"];
    ipBuffer = root["ip"];
    titleBuffer = root["title"];
    //strncpy(contentBuffer, root["body"], 800);
    replyCount = root["replyCount"];
    voteCount = root["voteCount"];
    
    fp.close();
    free(json); 
    
    server.sendContent(String() + "<form method='POST' action='thread'><tr style='height:20px'><tr><td>"
      "<input type='hidden' name='id' value='/data/" + String(threadIDBuffer) + ".txt'></input>"
      "<input type='submit' value='Thread #" + String(threadIDBuffer) + "'></input>&emsp;&emsp;&emsp;<i>" 
      + String(titleBuffer) + "</i><br><br><small><b>" + String(ipBuffer) + 
      "</b></small>&emsp;&emsp;&emsp;<small>Votes: " + String(voteCount) +"</small>&emsp;&emsp;&emsp;<small>Replies: "
      + String(replyCount) + "</small></td></tr><tr style='height:10px'></tr></form>"
    );    
  }
  
  WiFiClient client = server.client();
  server.sendContent(
    "<tr style='height:20px'></tr></table></form></td></tr></table>"
    "<br /><h4>Post</h4><form method='POST' action='boardsave'>"
    "<input type='text' placeholder='title' maxlength='50' style='margin: 0px; width: 635px; "
    "z-index: auto; position: relative; line-height: normal; font-size: 13.3333px; transition: none; "
    "!important;' name='t'  spellcheck='true' required><br><br>"
    "<textarea style='margin: 0px; height: 175px; width: 635px;' placeholder='body' maxlength='800' name='b' spellcheck='true' required></textarea>"
    "<input type='hidden' name='ip' value='" + toStringIp(client.remoteIP()) + "'></input>"
    "<br><br><input type='submit' value='Submit'/></form>"
  ); 
  server.client().stop(); // Stop is needed because we sent no content length

}

void handleBoardSave(){
  Serial.println("Board save");
  char threadIDFile[14];
  sprintf(threadIDFile, "/data/%d.txt", threadID);
  File fp = SPIFFS.open(threadIDFile, "w+");   // Make new file titled with threadID

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["threadID"] = threadID;
  root["ip"] = server.arg("ip");
  root["voteCount"] = 0;
  root["replyCount"] = 0;
  root["title"] = server.arg("t");
  root["body"] = server.arg("b");
  JsonObject& replies = root.createNestedObject("replies");
  JsonArray& voteIP = root.createNestedArray("voteIP");
  root.printTo(fp);
  root.printTo(Serial);
  fp.print('\0');
  fp.close(); 
  threadID++;
  server.sendHeader("Location", "board", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
}

