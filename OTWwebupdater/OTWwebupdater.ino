#include <ETH.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WebSocketsClient.h>
#include <SocketIoClient.h>  

const char* host = "esp32";
IPAddress local_IP(192, 168, 0, 50);  
IPAddress gateway(192, 168, 0, 1);    
IPAddress subnet(255, 255, 255, 0);   
IPAddress primaryDNS(8, 8, 8, 8);     
IPAddress secondaryDNS(8, 8, 4, 4);   

WebServer server(80);
SocketIOclient socketIO;


const char* loginIndex = 
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr><td colspan=2><center><font size=4><b>ESP32 Login Page</b></font></center><br></td></tr>"
  "<tr><td>Username:</td><td><input type='text' size=25 name='userid'><br></td></tr>"
  "<tr><td>Password:</td><td><input type='Password' size=25 name='pwd'><br></td></tr>"
  "<tr><td><input type='submit' onclick='check(this.form)' value='Login'></td></tr>"
  "</table></form>"
  "<script>function check(form){if(form.userid.value=='admin' && form.pwd.value=='admin'){window.open('/serverIndex')}"
  "else { alert('Error Password or Username') }}</script>";


const char* serverIndex = 
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'><input type='submit' value='Update'></form>"
  "<div id='prg'>progress: 0%</div>"
  "<script>"
  "$('form').submit(function(e){e.preventDefault();var form=$('#upload_form')[0];var data=new FormData(form);"
  "$.ajax({url:'/update',type:'POST',data:data,contentType:false,processData:false,xhr:function(){"
  "var xhr=new window.XMLHttpRequest();xhr.upload.addEventListener('progress',function(evt){"
  "if(evt.lengthComputable){var per=evt.loaded/evt.total;$('#prg').html('progress: '+Math.round(per*100)+'%');}"
  "},false);return xhr;},success:function(d,s){console.log('success!')},error:function(a,b,c){}});"
  "});</script>";

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case sIOtype_CONNECT:
      Serial.println("[Socket.IO] Connected to server");
      break;
    case sIOtype_EVENT:
      Serial.printf("[Socket.IO] Event received: %s\n", payload);
      break;
    case sIOtype_DISCONNECT:
      Serial.println("[Socket.IO] Disconnected from server");
      break;
    default:
      Serial.printf("[Socket.IO] Unknown event: %d\n", type);
  }
}

void setupWebUpdater() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.begin();
}

void setup() {
  Serial.begin(115200);

  WiFi.disconnect(true);  // Disable WiFi
  WiFi.mode(WIFI_OFF);
  btStop();

  // Initialize Ethernet
  if (!ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
    Serial.println("Failed to initialize Ethernet");
    return;
  }

  if (!ETH.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Static IP configuration failed");
  }

  Serial.println("Starting Ethernet...");

  while (!ETH.linkUp()) {
    delay(1000);
    Serial.println("Waiting for Ethernet...");
  }

  Serial.print("Connected to Ethernet. IP address: ");
  Serial.println(ETH.localIP());

  setupWebUpdater();  // Start Web Updater

  socketIO.begin("192, 168, 0, 11", 80);  // Replace with your Socket.IO server address
  socketIO.onEvent(socketIOEvent);  // Listen for the "connect" event

  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
}

void loop() {
  server.handleClient();  // Handle Web Updater client requests
  socketIO.loop();        // Maintain Socket.IO connection
}