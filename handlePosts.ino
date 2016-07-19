void handleBoard() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head><title>Hacker Union</title></head>"
    "<body><center><br><h1>Discussion Board</h1><br>"
    "<table id='hnmain' border='0' cellpadding='0' cellspacing='0' width='85%' bgcolor='#f6f6ef'>"
    "<tr><td bgcolor='#ff6600'>"
    "<table border='0' cellpadding='0' cellspacing='0' width='100%' style='padding:2px'>"
    "<tr><td style='width:18px;padding-right:4px'>"
    "<a href='/'><img width='18' height='18' style='border:1px #ffffff solid;background-color:#D7F1FA;'></a>"
    "</td><td style='line-height:15pt; height:10px;'><span class='pagetop'><b>&nbsp; Hacker Union &nbsp;&nbsp;</b>"
    "<a href='/board'>posts</a> | <a href='/wifi'>wifi</a> | <a href='/'>about</a> "
    "</span></td></tr></table></td></tr><tr><td><table border='1' width='100%' bordercolor='#f60'></td></tr>"  //blackBorder 1
    "<tr style='height:20px'></tr>"
  );
  char titleBuffer[50];
  char threadIDBuffer[4];
  char ipBuffer[16];
  int titleIndex;
  int threadIDIndex;
  int ipIndex;
  
  Serial.println("Handle Board");
  Dir dir = SPIFFS.openDir("/data");
  while (dir.next()) {
    //Serial.println(dir.fileName());
    File fp = dir.openFile("r");
    threadIDIndex = fp.readBytesUntil('\0', threadIDBuffer, 4);
    threadIDBuffer[threadIDIndex] = '\0';
    titleIndex = fp.readBytesUntil('\0', titleBuffer, 50);
    titleBuffer[titleIndex] = '\0';
    ipIndex = fp.readBytesUntil('\0', ipBuffer, 16);
    ipBuffer[ipIndex] = '\0';

    char path[20];
    char voteBuf[4] = "0";
    dir.fileName().toCharArray(path,20);
    memmove(path + 10, path + 5, 20 - 10);
    strcpy(path + 5, "/meta/");
    Serial.println(path);
    
    if (SPIFFS.exists(path)) {
      Serial.println("fp_meta");  
      File fp_meta = SPIFFS.open(path, "r");
      fp_meta.seek(4,SeekEnd);
      fp_meta.readBytesUntil('\0', voteBuf, 4);
      Serial.println(voteBuf);
      fp_meta.close(); 
    }
    fp.close();
    server.sendContent(String() + "<form method='POST' action='thread'><tr style='height:20px'><tr><td>"
      "<input type='hidden' name='id' value='/data/" + String(threadIDBuffer) + ".txt'></input>"
      "<input type='submit' value='Thread #" + String(threadIDBuffer) + "'></input>&emsp;&emsp;&emsp;<mark><i>" 
      + String(titleBuffer) + "</mark></i><br><br><small><b>" + String(ipBuffer) + 
      "</b></small>&emsp;&emsp;&emsp;<small>Votes:" + String(voteBuf) +"</small>&emsp;&emsp;&emsp;<small>Replies: 0 </small>"
      "</td></tr><tr style='height:10px'></tr></form>"
    );

  }
  WiFiClient client = server.client();
  server.sendContent(
    "<tr style='height:20px'></tr></table></form></td></tr></table>"
    "<br /><h4>Post</h4><form method='POST' action='boardsave'>"
    "<input type='text' placeholder='title' maxlength='50' size='100%' name='t' required><br><br>"
    "<textarea style='margin: 0px; height: 175px; width: 635px;' placeholder='body' maxlength='800' name='b' required></textarea>"
    "<input type='hidden' name='ip' value='" + toStringIp(client.remoteIP()) + "'></input>"
    "<br><br><input type='submit' value='Submit'/></form>"
  ); 
  server.client().stop(); // Stop is needed because we sent no content length
}

void handleBoardSave(){
  Serial.println("Board save");
  char threadIDFile[14];
  sprintf(threadIDFile, "/data/%d.txt", threadID);
  File fp = SPIFFS.open(threadIDFile, "a+");   // Make new file titled with threadID
  
  fp.print(threadID);
  fp.print('\0');
  fp.print(server.arg("t"));
  fp.print('\0');
  fp.print(server.arg("ip"));
  fp.print('\0');
  fp.print(server.arg("b"));
  fp.print('\0');

  threadID++;
  server.sendHeader("Location", "board", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  fp.close();
  server.client().stop(); // Stop is needed because we sent no content length
}

void handleThread() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head><title>Hacker Union</title></head>"
    "<body><center><br><h1>Thread</h1><br>"
    "<table id='hnmain' border='0' cellpadding='0' cellspacing='0' width='85%' bgcolor='#f6f6ef'>"
    "<tr><td bgcolor='#ff6600'>"
    "<table border='0' cellpadding='0' cellspacing='0' width='100%' style='padding:2px'>"
    "<tr><td style='width:18px;padding-right:4px'>"
    "<a href='/'><img width='18' height='18' style='border:1px #ffffff solid;background-color:#D7F1FA;'></a>"
    "</td><td style='line-height:15pt; height:10px;'><span class='pagetop'><b>&nbsp; Hacker Union &nbsp;&nbsp;</b>"
    "<a href='/board'>posts</a> | <a href='/wifi'>wifi</a> | <a href='/'>about</a> "
    "</span></td></tr></table></td></tr><tr><td><table border='1' width='100%' bordercolor='#f60'></td></tr>"
    "<tr style='height:20px'></tr>"
  );
  char threadIDBuffer[4];
  char titleBuffer[50];
  char contentBuffer[800];
  char ipBuffer[16];
  int threadIDIndex; 
  int titleIndex;
  int contentIndex;
  int ipIndex;
  Serial.println("Thread");
  Serial.println(server.arg("id"));
  WiFiClient client = server.client();
  File fp = SPIFFS.open(server.arg("id"), "r"); 
  /* Read OP */
  if (fp.available()){
    threadIDIndex = fp.readBytesUntil('\0', threadIDBuffer, 5);    
    titleIndex = fp.readBytesUntil('\0', titleBuffer, 50);
    ipIndex = fp.readBytesUntil('\0', ipBuffer, 50);
    contentIndex = fp.readBytesUntil('\0', contentBuffer, 800);
    threadIDBuffer[threadIDIndex] = '\0';
    titleBuffer[titleIndex] = '\0'; 
    contentBuffer[contentIndex]= '\0';
    ipBuffer[ipIndex] = '\0';
    server.sendContent(String() + "<tr style='height:20px'></tr><tr><td><form method='POST' action='threadsave' style='display:inline;'>");
    server.sendContent(String() + "<input type='hidden' name='ip' value='" + toStringIp(client.remoteIP()) + "'></input>");
    server.sendContent(String() +"<input type='submit' value='+'/></form>&emsp;&emsp;&emsp;Thread #" + String(threadIDBuffer));
    server.sendContent(String() + "&emsp;&emsp;&emsp;"+ String(ipBuffer) + "</td></tr><tr><td><b>" + String(titleBuffer));
    server.sendContent(String() + "</b><br><br>" + String(contentBuffer) + "</td></tr><tr style='height:20px'></tr>");
    /*
    server.sendContent(String() + "<tr style='height:20px'></tr><tr><td> <a href= ''> + </a>&nbsp;<a href=''> - </a>&emsp;&emsp;&emsp;" );  
    server.sendContent(String() + "Thread #"+ String(threadIDBuffer) + "&emsp;&emsp;&emsp;"+ String(ipBuffer) +"</td></tr><tr><td><b>" + String(titleBuffer)); 
    server.sendContent(String() + "</b><br><br>" + String(contentBuffer) + "</td></tr><tr style='height:20px'></tr>");
  */}
  while (fp.available()){
    contentIndex = fp.readBytesUntil('\0', contentBuffer, 800);
    contentBuffer[contentIndex]= '\0';
    server.sendContent(String() + "<tr><td>" + String(contentBuffer) + "</td></tr><tr style='height:20px'></tr>");
  }
  fp.close();
  server.sendContent(
    "<tr style='height:20px'></tr></table></form></td></tr></table>"
    "<br /><form method='POST' action='threadsave'><h4>Reply</h4>"
    "<input type='hidden' name='id' value='/data/" + String(threadIDBuffer) + ".txt'></input>"
    "<textarea style='margin: 0px; height: 175px; width: 635px;' placeholder='reply' maxlength='800' name='r' required></textarea>"
    "<br><br><input type='submit' value='Submit'/></form>"
  ); 
  server.client().stop(); // Stop is needed because we sent no content length
}

void handleThreadSave(){
  Serial.println("Thread save");
  if (server.arg("ip")) {   
    char metaData[14]; char temp[4]; 
    server.arg("id").toCharArray(temp, 4);
    int id_ = atoi(temp);
    sprintf(metaData, "/data/meta/%d.txt", id_);
    if (!alreadyVoted(metaData, server.arg("ip"))) {
      File fp = SPIFFS.open(metaData, "a+");
      fp.print(server.arg("ip"));
      fp.print('\0');
      fp.close();
    }
  } else {
    File fp = SPIFFS.open(server.arg("id"), "a+");   // append to file titled with threadID
    fp.print(server.arg("r"));
    fp.print('\0');
    fp.close();
    Serial.print(server.arg("id"));
  }
  
  //server.send(server.arg("id"));
  server.sendHeader("Location", "board", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  
  server.client().stop(); // Stop is needed because we sent no content length
}

