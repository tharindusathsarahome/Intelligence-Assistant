#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const char *ssid = "Xiaomi";
const char *password = "CHANNA2001";

const int led = 4;
WiFiServer server(80);
SoftwareSerial NodeMCU(D2,D3);

IPAddress local_IP(192, 168, 114, 100); // Set your Static IP address
IPAddress gateway(192, 168, 114, 200); // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);	
	NodeMCU.begin(9600);
	pinMode(D2,INPUT);
	pinMode(D3,OUTPUT);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  Serial.print("\nConnecting to.");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("..");
  }
  Serial.println("\nNodemcu(esp8266) is connected to the ssid");
  Serial.println(WiFi.localIP());
  server.begin();
  delay(1000);
}

void loop()
{
  WiFiClient client;
  client = server.available();

  if (client){

    String request = client.readStringUntil('\n');
    if (request.startsWith("GET")) {
      int paramStart = request.indexOf("?") + 1;
      int paramEnd = request.indexOf(" ", paramStart);
      String params = request.substring(paramStart, paramEnd);
      
      String errValueX = getValue(params, "mx_err");
      String errValueY = getValue(params, "my_err");
      String mInitValue = getValue(params, "m_init");
      String stopVal = getValue(params, "m_stop");
      String servoBase = getValue(params, "servo_base");
      String servoGrip = getValue(params, "servo_grip");
      String servo1 = getValue(params, "servo_1");
      String servo2 = getValue(params, "servo_2");
      String servoRelax = getValue(params, "servo_relax");

      String parameterList = "m_stop:" + String(stopVal) + ",m_init:" + String(mInitValue) + ",mx_err:" + String(errValueX) + ",my_err:" + String(errValueY) + ",servo_relax:" + String(servoRelax) + ",servo_base:" + String(servoBase) + ",servo_grip:" + String(servoGrip) + ",servo_1:" + String(servo1) + ",servo_2:" + String(servo2) + ",##:0";
      NodeMCU.println(parameterList);
      Serial.println(parameterList);

      // sendResponse(client, "200OK");
    }
    delay(10);
  }

  if (NodeMCU.available()) {
    String receivedString = NodeMCU.readStringUntil('\n');
    Serial.println(receivedString);
    sendResponse(client, receivedString);
  }
}

void sendResponse(WiFiClient client, String content) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println();
  client.println(content);
}

String getValue(String data, String key) {
  String separator = "=";
  int keyStart = data.indexOf(key + separator);
  if (keyStart != -1) {
    keyStart += key.length() + separator.length();
    int keyEnd = data.indexOf("&", keyStart);
    if (keyEnd == -1) {
      keyEnd = data.length();
    }
    return data.substring(keyStart, keyEnd);
  }
  return "";
}