#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

String ssid = "Solar Control";

WiFiServer server(80);

SoftwareSerial mySerial(4,5);

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
//  Serial.println("ooo");
//  //mySerial.flush();
//  
//  byte x[4];
//
//  char idString[] = "mofeed";
//  int counter = 0;
//  
//  while (true)
//  {
//    while (mySerial.available() == 0)
//      delay(1);
//
//    if (counter == 6)
//    {
//      Serial.println("found 6");
//      break;
//    }
//    else if (mySerial.read() == idString[counter])
//    {
//      Serial.println("found " + idString[counter]);
//      counter++;
//      continue;
//    }
//    else
//    {
//      Serial.println("reset .. counter");
//      counter = 0;
//    }
//  }
//
//  while (mySerial.available() == 0)
//      delay(1);
//      
//  
//  for (int i = 0; i < 4; ++i)
//  {
//    while (mySerial.available() == 0)
//      delay(1);
//      
//    x[i] = mySerial.read();
//  }

  return mySerial.read() / 5;
}
