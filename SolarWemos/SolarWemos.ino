#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

String ssid = "Solar Control";

WiFiServer server(80);

SoftwareSerial mySerial(14, 2);

void setupWiFi(String password)
{
  WiFi.mode(WIFI_AP);

  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = ssid + "-" + macID;


  WiFi.softAP(AP_NameString.c_str(), password.c_str());
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  setupWiFi("12345678");

  mySerial.begin(115200);

  server.begin();
}


void loop() {

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  //  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String req = client.readStringUntil('\r');
 
  client.flush();
  
  String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

  Serial.println(req);

  if (req.indexOf("/v") != -1)
  {
    
    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    html += String(readFloat());
  }
  else if (req.indexOf("/c") != -1)
  {
    html += "1234";
  }

  client.print(html);
  delay(1);


}



float readFloat()
{
  mySerial.flush();
  while (mySerial.available() == 0)
      delay(1);


  int xxxx = mySerial.read();
  float yyyy = xxxx / 51.0f;
  Serial.println("xx: " + String(xxxx));
  Serial.println("yy: " + String(yyyy));
  return yyyy;
}
